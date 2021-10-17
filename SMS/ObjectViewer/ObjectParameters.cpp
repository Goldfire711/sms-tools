#include "ObjectParameters.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "../../Memory/Memory.h"

extern QTimer* g_timer_100ms;
using namespace memory;

ObjectParameters::ObjectParameters(QWidget* parent)
  : QWidget(parent) {
  ui.setupUi(this);
}

ObjectParameters::~ObjectParameters() {
}

void ObjectParameters::set_label(QString string) {
  ui.label->setText(string);
}

void ObjectParameters::show_parameters(u32 address, s64 index) {
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
  name += " <" + class_name + ">";

  ui.label->setText(name);

  // class名からoffsets(offset,type,nameのarray)をロード なければ_defaultをロード


  // table viewでjsonから得たoffsetsを表示
}
