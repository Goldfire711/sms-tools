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
  // vtable, name���擾(+0x0, +0x4)
  // vtable�̒l����json�����[�h
  // "(index): (name)"����̃��x���ɃZ�b�g
  // table view��json���瓾���p�����[�^�[��\��
}
