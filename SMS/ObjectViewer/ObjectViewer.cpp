#include "ObjectViewer.h"
#include "../../DolphinProcess/DolphinAccessor.h"
#include "../../Common/CommonUtils.h"
#include "../../Memory/Memory.h"
#include "ObjectViewerItem.h"
#include "Settings/Settings.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QJsonArray>
#include <QTextCodec>
#include <QHash>
#include <QMenu>
#include <QSettings>
#include <QShortcut>
#include <fstream>
#include <QDebug>

#include <json.hpp>

extern QTimer* g_timer_100ms;
using json = nlohmann::json;
using namespace memory;

ObjectViewer::ObjectViewer(QWidget* parent)
  : QMainWindow(parent) {
  ui.setupUi(this);

  // read json 
  const QString file_path = "SMS/Resources/ObjectViewer.json";
  QFile file(file_path);
  if (!file.open(QIODevice::ReadOnly)) {
    ui.label->setText("Couldn't open " + file_path);
    return;
  } else {
    ui.label->setText(file_path);
  }
  QJsonDocument load_doc(QJsonDocument::fromJson(file.readAll()));
  file.close();

  // set json to model_ and set model_ to tree_object
  model_ = new ObjectViewerModel(load_doc.object(), this);
  ui.tree_object->setModel(model_);
  ui.tree_object->setColumnWidth(0, 318);
  ui.tree_object->setColumnWidth(2, 72);

  connect(ui.button_refresh, &QPushButton::clicked, this, &ObjectViewer::refresh);
  connect(g_timer_100ms, &QTimer::timeout, model_, QOverload<>::of(&ObjectViewerModel::on_update));

  ui.tree_object->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui.tree_object, &QWidget::customContextMenuRequested, this, &ObjectViewer::show_context_menu);

  auto* shortcut_copy = new QShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_C), ui.tree_object);
  connect(shortcut_copy, &QShortcut::activated, this, [=] {
    copy_as_dmw_format();
    });

  show_widget_object_parameters();
  connect(ui.tree_object, &QAbstractItemView::activated, this, &ObjectViewer::on_tree_object_clicked);
  connect(ui.tree_object, &QAbstractItemView::clicked, this, &ObjectViewer::on_tree_object_clicked);

  const QSettings& settings = Settings::GetQSettings();
  restoreState(settings.value("object_viewer/state").toByteArray());
  restoreGeometry(settings.value("object_viewer/geometry").toByteArray());

  refresh();
}

ObjectViewer::~ObjectViewer() {
  delete model_;
}

void ObjectViewer::closeEvent(QCloseEvent* event) {
  QSettings& settings = Settings::GetQSettings();
  settings.setValue("object_viewer/state", saveState());
  settings.setValue("object_viewer/geometry", saveGeometry());

  disconnect(g_timer_100ms, &QTimer::timeout, model_, QOverload<>::of(&ObjectViewerModel::on_update));
  if (object_parameters_) {
    object_parameters_->close();
    delete object_parameters_;
    object_parameters_ = nullptr;
  }
}

void ObjectViewer::showEvent(QShowEvent* event) {
  connect(g_timer_100ms, &QTimer::timeout, model_, QOverload<>::of(&ObjectViewerModel::on_update));
  show_widget_object_parameters();

  const QSettings& settings = Settings::GetQSettings();
  restoreState(settings.value("object_viewer/state").toByteArray());
  restoreGeometry(settings.value("object_viewer/geometry").toByteArray());
  object_parameters_->restoreGeometry(settings.value("object_parameters/geometry").toByteArray());
  object_parameters_->setFloating(settings.value("object_parameters/floating").toBool());

  // TODO auto refresh
  // connect(g_timer_100ms, &QTimer::timeout, this, QOverload<>::of(&ObjectViewer::on_update));
}

void ObjectViewer::refresh() {
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  // delete model_
  if (model_ != nullptr) {
    disconnect(g_timer_100ms, &QTimer::timeout, model_, QOverload<>::of(&ObjectViewerModel::on_update));
    ui.tree_object->setModel(nullptr);
    delete model_;
    model_ = nullptr;
  }

  // read ObjectViewer.json
  const QString file_path = "SMS/Resources/ObjectViewer.json";
  QFile file3(file_path);
  if (!file3.open(QIODevice::ReadOnly)) {
    ui.label->setText("Couldn't open " + file_path);
    return;
  }
  ui.label->setText(file_path);
  const QJsonDocument load_doc3(QJsonDocument::fromJson(file3.readAll()));
  file3.close();
  QJsonObject json = load_doc3.object();
  QJsonArray json_array = json["objects"].toArray();

  // read name,address from ObjectViewer_managers.json
  QFile file("SMS/Resources/ObjectViewer_managers.json");
  if (!file.open(QIODevice::ReadOnly))
    return;
  const QJsonDocument load_doc(QJsonDocument::fromJson(file.readAll()));
  file.close();
  QJsonArray json_managers = load_doc["managers"].toArray();
  QJsonObject json_template = load_doc["object_template"].toObject();

  // setup hash for scanned manager
  struct Manager {
    QString name;
    QJsonObject json_object;
  };
  QHash<u32, Manager> hash_manager;
  for (auto value_ref : json_managers) {
    auto json_manager = value_ref.toObject();

    u32 vtable = json_manager["vtable"].toString().toUInt(nullptr, 16);

    Manager manager;
    manager.name = json_manager["name"].toString();
    if (!json_manager["object"].isObject()) {
      manager.json_object = json_template;
    } else {
      manager.json_object = json_manager["object"].toObject();
    }

    hash_manager.insert(vtable, manager);
  }

  // scan managers (gpConductor(0x8040a6e8)を使用)
  // +0x0: next manager, +0x4: prev, 0x8: manager
  // の双方向連結リスト
  u32 addr = read_u32(0x8040a6e8);
  s32 count = read_u32(addr + 0x14);
  u32 head = read_u32(addr + 0x18);
  u32 l = head;
  for (s64 i = 0; i < count-1; i++) {
    l = read_u32(l);
    u32 m = read_u32(l + 0x8);
    u32 vt = read_u32(m);
    auto* manager = &hash_manager[vt];
    manager->json_object["value"] = QString::number(m, 16);
    json_array.append(manager->json_object);
  }

  // set model_ to tree_object
  json.insert("objects", json_array);
  model_ = new ObjectViewerModel(json, this);
  ui.tree_object->setModel(model_);
  model_->on_update();
  ui.tree_object->resizeColumnToContents(0);
  ui.tree_object->setColumnWidth(2, 72);
  connect(g_timer_100ms, &QTimer::timeout, model_, QOverload<>::of(&ObjectViewerModel::on_update));
  emit model_->layoutChanged();
}

void ObjectViewer::show_context_menu(const QPoint& pos) {
  const QModelIndex index = ui.tree_object->indexAt(pos);
  if (index != QModelIndex())
    return;

  auto* menu = new QMenu(this);

  //const ObjectViewerItem* item = model_->get_item(index);
  //auto* action_copy = new QAction("copy as dmw format", this);
  //connect(action_copy, &QAction::triggered, this, [=] {
  //  copy_as_dmw_format();
  //});
  //menu->addAction(action_copy);

  menu->popup(ui.tree_object->viewport()->mapToGlobal(pos));
}

// todo
void ObjectViewer::copy_as_dmw_format() {
  const QModelIndexList selection = ui.tree_object->selectionModel()->selectedRows();
  if (selection.count() == 0)
    return;

  selection[0];
}

void ObjectViewer::on_update() {
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;
  const u32 conductor = read_u32(0x8040a6e8);
  const s32 count = read_s32(conductor + 0x14);
  if (conductor != prev_conductor_ || count != prev_count_)
    refresh();
  prev_conductor_ = conductor;
  prev_count_ = count;
}

void ObjectViewer::show_widget_object_parameters() {
  if (object_parameters_ == nullptr)
    object_parameters_ = new ObjectParameters(this);
  object_parameters_->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::RightDockWidgetArea, object_parameters_);
}

void ObjectViewer::on_tree_object_clicked(const QModelIndex& index) {
  QSettings settings("settings.ini", QSettings::IniFormat);
  settings.setValue("object_viewer/geometry", saveGeometry());
  settings.setValue("object_viewer/state", saveState());

  auto* item = static_cast<ObjectViewerItem*>(index.internalPointer());
  const u32 address = item->value_.toUInt();
  if (0x80000000 <= address && address <= 0x817fffff)
    object_parameters_->show_parameters(address, item->index_);
}