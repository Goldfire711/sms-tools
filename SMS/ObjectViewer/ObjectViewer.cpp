#include "ObjectViewer.h"
#include "../../DolphinProcess/DolphinAccessor.h"
#include "../../Common/CommonUtils.h"
#include "../../Memory/Memory.h"
#include "ObjectViewerItem.h"

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

extern QTimer* g_timer_100ms;

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

  connect(ui.button_scan_managers, &QPushButton::clicked, this, &ObjectViewer::scan_managers);
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

  QSettings settings("settings.ini", QSettings::IniFormat);
  restoreGeometry(settings.value("object_viewer/geometry").toByteArray());
  restoreState(settings.value("object_viewer/state").toByteArray());
}

ObjectViewer::~ObjectViewer() {
  delete model_;
}

void ObjectViewer::closeEvent(QCloseEvent* event) {
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
  //connect(g_timer_100ms, &QTimer::timeout, this, QOverload<>::of(&ObjectViewer::on_update));
}

void ObjectViewer::scan_managers() {
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

  // read entire memory
  constexpr u32 mem_start = 0x80414020;
  constexpr u32 mem_end = 0x81800000;
  constexpr u32 mem_size = mem_end - mem_start;
  u8* scan_ram_cache = new u8[mem_size];
  if (!DolphinComm::DolphinAccessor::readFromRAM(mem_start & 0x7fffffff, scan_ram_cache, mem_size, false)) {
    delete[] scan_ram_cache;
    return;
  }

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

    u32 vtable_bswap = Common::bSwap32(json_manager["vtable"].toString().toUInt(nullptr, 16));

    Manager manager;
    manager.name = json_manager["name"].toString();
    if (!json_manager["object"].isObject()) {
      manager.json_object = json_template;
    } else {
      manager.json_object = json_manager["object"].toObject();
    }

    hash_manager.insert(vtable_bswap, manager);
  }

  // scan managers
  QByteArray search_term = QTextCodec::codecForName("Shift-JIS")->fromUnicode("Manager");
  search_term.append('\0');
  for (u32 i = 0; i < (mem_size - 0x100); i++) {
    u8* memory_candidate = &scan_ram_cache[i];
    if (std::memcmp(memory_candidate, search_term.data(), 8) == 0) {
      const s64 vtable_offset = 11 - ((i + 3) & 0b11);
      u32 vtable;
      std::memcpy(&vtable, &memory_candidate[vtable_offset], 4);
      if (hash_manager.contains(vtable)) {
        auto* manager = &hash_manager[vtable];
        manager->json_object["value"] = QString::number(i + vtable_offset + mem_start, 16);
        //manager->json_object["name"].toObject()["name"] = manager->name;
        //manager.json_object["name"] = manager.name;
        json_array.append(manager->json_object);
      }
    }
  }

  delete[] scan_ram_cache;

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
  ui.label->setText(QString::number(ui.tree_object->columnWidth(0)));
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