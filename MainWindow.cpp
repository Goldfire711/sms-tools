#include "MainWindow.h"

#include "DolphinProcess/DolphinAccessor.h"
#include "Common/CommonUtils.h"
#include "Memory/Memory.h"

QTimer* g_timer;

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
  connect(ui.button_hook, &QPushButton::clicked, this, &MainWindow::hook_unhook_attempt);
  connect(ui.button_widget_spin, &QPushButton::clicked, this, &MainWindow::show_widget_spin);
  connect(ui.button_widget_object_viewer, &QPushButton::clicked, this, &MainWindow::show_widget_object_viewer);
  //connect(ui.button_widget_map_viewer, &QPushButton::clicked, this, &MainWindow::show_widget_map_viewer);
  //connect(ui.button_widget_fluff_manipulator, &QPushButton::clicked, this, &MainWindow::show_widget_fluff_manipulator);
  g_timer = new QTimer(this);
  connect(g_timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::on_update));
  hook_unhook_attempt();

  show_widget_object_viewer();
}

void MainWindow::hook_unhook_attempt() {
  if (ui.button_hook->isChecked()) {
    DolphinComm::DolphinAccessor::hook();
  } else {
    g_timer->stop();
    DolphinComm::DolphinAccessor::unHook();
  }
  switch (DolphinComm::DolphinAccessor::getStatus()) {
  case DolphinComm::DolphinAccessor::DolphinStatus::hooked: {
    ui.label_dolphin_status->setText(
      tr("Start address: ") +
      QString::number(DolphinComm::DolphinAccessor::getEmuRAMAddressStart(), 16).toUpper());
    ui.button_hook->setText(tr("Unhook"));
    ui.button_hook->setChecked(true);
    g_timer->start(16);

    QVariant test = memory::read_string(0x8040A4D8, { 0x18, 0, 4, 0 }, 32);
    ui.button_widget_fluff_manipulator->setText(test.toString());
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::notRunning: {
    ui.label_dolphin_status->setText(tr("Dolphin is not running"));
    ui.button_hook->setText(tr("Hook"));
    ui.button_hook->setChecked(false);
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::noEmu: {
    ui.label_dolphin_status->setText(tr("SMS is not running"));
    ui.button_hook->setText(tr("Hook"));
    ui.button_hook->setChecked(false);
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::unHooked: {
    ui.label_dolphin_status->setText(tr("Unhooked"));
    ui.button_hook->setText(tr("Hook"));
    ui.button_hook->setChecked(false);
    break;
  }
  }
}

void MainWindow::on_update() {
  count_++;
  ui.button_widget_fluff_manipulator->setText(QString::number(count_));
  if (memory::read_string(0, 7) != "GMSJ01" && update_ == true) {
    g_timer->stop();
    hook_unhook_attempt();
    update_ = false;
  } else {
    update_ = true;
  }
}


void MainWindow::show_widget_spin() {
  if (!sms_spin_) {
    sms_spin_ = new Spin(nullptr);
    g_timer->connect(g_timer, SIGNAL(timeout()), sms_spin_, SLOT(update()));
  }
  sms_spin_->show();
  sms_spin_->raise();
  sms_spin_->activateWindow();
}

void MainWindow::show_widget_object_viewer() {
  if (!sms_object_viewer_) {
    sms_object_viewer_ = new ObjectViewer(nullptr);
    //connect(g_timer, &QTimer::timeout, sms_object_viewer_, QOverload<>::of(&ObjectViewer::on_update));
  }
  sms_object_viewer_->show();
  sms_object_viewer_->raise();
  sms_object_viewer_->activateWindow();
  
}
