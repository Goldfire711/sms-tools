#pragma once

#include <QMainWindow>
#include "ui_TestMainWindow.h"
#include "../SMS/RNGManipulator/RNGManipulator.h"
#include "ReadWriteTest/ReadWriteTest.h"

class TestMainWindow : public QMainWindow {
Q_OBJECT

public:
  TestMainWindow(QWidget* parent = Q_NULLPTR);
  ~TestMainWindow() override;

  void closeEvent(QCloseEvent* event) override;
  void show_rng_manipulator_();
  void show_read_write_test();

private:
  Ui::TestMainWindow ui;

  RNGManipulator* rng_manipulator_ = nullptr;
  ReadWriteTest* read_write_test_ = nullptr;
};
