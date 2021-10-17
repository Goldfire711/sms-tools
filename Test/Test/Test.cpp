#include "Test.h"
#include <QStringList>

Test::Test(QWidget* parent)
  : QWidget(parent) {
  ui.setupUi(this);

  QStringList lists = { "1", "2", "3", "4", "5" };
  if (lists.contains("1"))
    ui.label->setText("true");
  
}

Test::~Test() {
}
