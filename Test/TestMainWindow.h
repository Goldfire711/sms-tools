#pragma once

#include <QMainWindow>
#include "ui_TestMainWindow.h"
#include "../SMS/RNGManipulator/RNGManipulator.h"

class TestMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  TestMainWindow(QWidget *parent = Q_NULLPTR);
  ~TestMainWindow();

  void show_rng_manipulator_();

private:
  Ui::TestMainWindow ui;

  RNGManipulator* rng_manipulator_ = nullptr;
};
