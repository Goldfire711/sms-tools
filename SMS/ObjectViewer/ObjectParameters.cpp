#include "ObjectParameters.h"

extern QTimer* g_timer_100ms;

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
  // vtableの値からjsonをロード
  // "(index): (name)"を上のラベルにセット
  // table viewでjsonから得たパラメーターを表示
}
