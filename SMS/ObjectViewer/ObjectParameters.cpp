#include "ObjectParameters.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTimer>

#include "../../Memory/Memory.h"

QJsonDocument g_json_classes;
extern QTimer* g_timer_100ms;
using namespace memory;

ObjectParameters::ObjectParameters(QWidget* parent)
  : QDockWidget(parent) {
  ui.setupUi(this);

  QFile file2("SMS/Resources/ObjectParameters.json");
  if (!file2.open(QIODevice::ReadOnly))
    return;
  g_json_classes = QJsonDocument(QJsonDocument::fromJson(file2.readAll()));
  file2.close();

  model_ = new ObjectParametersModel(&items_, this);
  ui.table_parameters->setModel(model_);
  //ui.table_parameters->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  connect(g_timer_100ms, &QTimer::timeout, this, QOverload<>::of(&ObjectParameters::refresh_items));
  connect(ui.button_edit_parameters, &QPushButton::clicked, this, &ObjectParameters::show_edit_parameters_dialog);
}

ObjectParameters::~ObjectParameters() {
}

void ObjectParameters::set_label(QString string) {
  ui.label->setText(string);
}

void ObjectParameters::show_parameters(u32 address, s64 index) {
  address_ = address;
  // vtable, nameを取得(+0x0, +0x4)
  u32 vtable = read_u32(address);
  QString name = read_string(address + 0x4, { 0 }, 100);
  if (index != -1)
    name = QString::number(index) + ": " + name;

  // vtableの値からclass名をロード(json)
  QFile file("SMS/Resources/vtable_to_class_JP.json");
  if (!file.open(QIODevice::ReadOnly))
    return;
  const QJsonDocument load_doc(QJsonDocument::fromJson(file.readAll()));
  file.close();
  const QJsonObject vtable_to_class = load_doc["Class"].toObject();
  const QString class_name = vtable_to_class[QString::number(vtable, 16)].toString();
  name += " <" + class_name + "> (0x" + QString::number(address, 16).toUpper() + ")";

  ui.label->setText(name);

  // class名からoffsets(offset,type,nameのarray)をロード なければ_defaultをロード
  QJsonObject json_class;
  items_.clear();
  if (!g_json_classes[class_name].isUndefined()) {
    json_class = g_json_classes[class_name].toObject();
    load_items_from_json(json_class, class_name);
  }
  else {
    json_class = g_json_classes["_default"].toObject();
    load_items_from_json(json_class, "_default");
  }
  refresh_items();
  ui.table_parameters->resizeColumnsToContents();
}

void ObjectParameters::load_items_from_json(const QJsonObject& json, const QString& class_name, const u32 base_offset) {
  const QJsonArray offsets = json["offsets"].toArray();
  const QStringList string_types = { "u8", "u16", "u32", "s8", "s16", "s32", "float", "string" };
  for (auto offset : offsets) {
    QJsonObject json_offset = offset.toObject();
    QString string_type = json_offset["type"].toString();
    if (string_types.contains(string_type)) {
      items_.append(ObjectParametersItem(address_, json_offset, class_name, base_offset));
    }
    else if (string_type.right(1) == "*") {
      items_.append(ObjectParametersItem(address_, json_offset, class_name, base_offset, true));
    }
    else {
      if (g_json_classes[string_type].isUndefined())
        continue;
      QJsonObject json_class = g_json_classes[string_type].toObject();
      load_items_from_json(json_class, string_type, base_offset + json_offset["offset"].toString().toUInt(nullptr, 16));
    }
  }
}

void ObjectParameters::refresh_items() {
  for (s64 i = 0; i < items_.size(); i++) {
    items_[i].read_memory();
  }
  model_->update_list();
}

void ObjectParameters::show_edit_parameters_dialog() {
  //auto* edit_parameters = new EditParametersDialog(g_json_classes, this);
  //edit_parameters->exec();
  //delete edit_parameters;
  EditParametersDialog edit_parameters(g_json_classes, this);
  edit_parameters.exec();
}
