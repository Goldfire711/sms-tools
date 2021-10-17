#pragma once

#include <QWidget>
#include "ui_Test.h"

class Test : public QWidget {
Q_OBJECT

public:
  Test(QWidget* parent = Q_NULLPTR);
  ~Test() override;

private:
  Ui::Test ui;
};
