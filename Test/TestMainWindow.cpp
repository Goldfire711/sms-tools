#include "TestMainWindow.h"

TestMainWindow::TestMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  ui.setupUi(this);
  connect(ui.button_rng_manupulator, &QPushButton::clicked, this, &TestMainWindow::show_rng_manipulator_);
}

TestMainWindow::~TestMainWindow()
{
}

void TestMainWindow::show_rng_manipulator_() {
  if (rng_manipulator_ == nullptr)
    rng_manipulator_ = new RNGManipulator();
  rng_manipulator_->show();
  rng_manipulator_->raise();
  rng_manipulator_->activateWindow();
}