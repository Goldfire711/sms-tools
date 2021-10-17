#include "ObjectParameters.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTimer>

#include "../../Memory/Memory.h"

extern QTimer* g_timer_100ms;
using namespace memory;

ObjectParameters::ObjectParameters(QWidget* parent)
  : QWidget(parent) {
  ui.setupUi(this);

  model_ = new ObjectParametersModel(&items_, this);
  ui.table_parameters->setModel(model_);

  connect(g_timer_100ms, &QTimer::timeout, this, QOverload<>::of(&ObjectParameters::refresh_items));

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
  name += " <" + class_name + "> (0x" + QString::number(address, 16) + ")";

  ui.label->setText(name);

  // class名からoffsets(offset,type,nameのarray)をロード なければ_defaultをロード
  QFile file2("SMS/Resources/ObjectParameters.json");
  if (!file2.open(QIODevice::ReadOnly))
    return;
  json_classes_ = QJsonDocument(QJsonDocument::fromJson(file2.readAll()));
  file2.close();
  QJsonObject json_class;
  items_.clear();
  if (!json_classes_[class_name].isUndefined()) {
    json_class = json_classes_[class_name].toObject();
    load_items_from_json(json_class, class_name);
  } else {
    json_class = json_classes_["_default"].toObject();
    load_items_from_json(json_class, "_default");
  }

  // table viewでjsonから得たoffsetsを表示
}

void ObjectParameters::load_items_from_json(const QJsonObject& json, QString class_name) {
  const QJsonArray offsets = json["offsets"].toArray();
  const QStringList string_types = { "u8", "u16", "u32", "s8", "s16", "s32", "float", "string" };
  for (auto offset : offsets) {
    QJsonObject json_offset = offset.toObject();
    QString string_type = json_offset["type"].toString();
    if (string_types.contains(string_type)) {
      items_.append(ObjectParametersItem(address_, json_offset, class_name));
    } else {
      if (json_classes_[string_type].isUndefined())
        continue;
      QJsonObject json_class = json_classes_[string_type].toObject();
      load_items_from_json(json_class, string_type);
    }
  }
}

void ObjectParameters::refresh_items() {
  for (s64 i = 0; i < items_.size(); i++) {
    items_[i].read_memory();
  }
  model_->update_list();
}
