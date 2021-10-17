#include "TestMainWindow.h"

#include <qevent.h>

TestMainWindow::TestMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  ui.setupUi(this);
  connect(ui.button_rng_manupulator, &QPushButton::clicked, this, &TestMainWindow::show_rng_manipulator_);
  connect(ui.button_read_write_test, &QPushButton::clicked, this, &TestMainWindow::show_read_write_test);
  connect(ui.button_test, &QPushButton::clicked, this, &TestMainWindow::show_test);
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

void TestMainWindow::show_read_write_test() {
  if (read_write_test_ == nullptr)
    read_write_test_ = new ReadWriteTest();
  read_write_test_->show();
  read_write_test_->raise();
  read_write_test_->activateWindow();
}

void TestMainWindow::show_test() {
  if (test_ == nullptr)
    test_ = new Test();
  test_->show();
  test_->raise();
  test_->activateWindow();
}


void TestMainWindow::closeEvent(QCloseEvent* event) {
  if (rng_manipulator_)
    rng_manipulator_->close();
  if (read_write_test_)
    read_write_test_->close();
  if (test_)
    test_->close();

  event->accept();
}
