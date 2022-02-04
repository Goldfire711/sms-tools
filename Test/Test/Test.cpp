#include "Test.h"
#include <QStringList>
#include <QTextCodec>
#include <QDebug>

Test::Test(QWidget* parent)
  : QWidget(parent) {
  ui.setupUi(this);

  QString text = "0102";
  QByteArray string_jis = QTextCodec::codecForName("Shift-JIS")->fromUnicode(text);
  string_jis.append('\0');
  for (int i = 0; i < string_jis.size(); i++) {
    qDebug() << (uint8_t)string_jis.data()[i];
  }
  qDebug() << "end";
  bool ok;
  short num = text.toShort(&ok, 16);
  qDebug() << *(int8_t*)&num << ok;
}

Test::~Test() {
}
