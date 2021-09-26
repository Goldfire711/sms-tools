#pragma once

#include <QWidget>
#include "ui_ReadWriteTest.h"
#include "ReadWriteTestItem.h"
#include "ReadWriteTestModel.h"

class ReadWriteTest : public QWidget {
Q_OBJECT

public:
  ReadWriteTest(QWidget* parent = Q_NULLPTR);
  ~ReadWriteTest() override;
  void refresh();

private:
  Ui::ReadWriteTest ui;
  ReadWriteTestModel* model_;
  QVector<ReadWriteTestItem> items_;
};
