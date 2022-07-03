#include "MainWindow.h"

#include <fstream>
#include <qevent.h>
#include <QDesktopWidget>
#include <json.hpp>
#include "DolphinProcess/DolphinAccessor.h"
#include "Common/CommonUtils.h"
#include "Memory/Memory.h"

QTimer* g_timer_100ms;
QTimer* g_timer_16ms;
nlohmann::json g_vtable_to_class;

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
  connect(ui.button_hook, &QPushButton::clicked, this, &MainWindow::hook_unhook_attempt);
  connect(ui.button_widget_spin, &QPushButton::clicked, this, &MainWindow::show_widget_spin);
  connect(ui.button_widget_object_viewer, &QPushButton::clicked, this, &MainWindow::show_widget_object_viewer);
  connect(ui.button_widget_map_viewer, &QPushButton::clicked, this, &MainWindow::show_widget_map_viewer);
  connect(ui.button_widget_fluff_manipulator, &QPushButton::clicked, this, &MainWindow::show_widget_fluff_manipulator);
  connect(ui.button_widget_chuuhana, &QPushButton::clicked, this, &MainWindow::show_widget_chuuhana);
  g_timer_100ms = new QTimer(this);
  g_timer_16ms = new QTimer(this);
  connect(g_timer_100ms, &QTimer::timeout, this, QOverload<>::of(&MainWindow::on_update));

  ui.button_hook->setChecked(true);
  hook_unhook_attempt();

  // test main window
  if (false) {
    test_main_window_ = new TestMainWindow(nullptr);
    test_main_window_->show();
    const auto pos = test_main_window_->pos();
    test_main_window_->move(pos.x() + 250, pos.y());
  }

  //show_widget_object_viewer();
  //show_widget_map_viewer();
}

void MainWindow::hook_unhook_attempt() {
  if (ui.button_hook->isChecked()) {
    DolphinComm::DolphinAccessor::hook();
  } else {
    g_timer_100ms->stop();
    g_timer_16ms->stop();
    DolphinComm::DolphinAccessor::unHook();
  }
  switch (DolphinComm::DolphinAccessor::getStatus()) {
  case DolphinComm::DolphinAccessor::DolphinStatus::hooked: {
    ui.label_dolphin_status->setText(
      tr("Start address: ") +
      QString::number(DolphinComm::DolphinAccessor::getEmuRAMAddressStart(), 16).toUpper());
    ui.button_hook->setText(tr("Unhook"));
    ui.button_hook->setChecked(true);
    g_timer_100ms->start(100);
    g_timer_16ms->start(16);
    version_setup();

    //QVariant test = memory::read_string(0x8040A4D8, { 0x18, 0, 4, 0 }, 32);
    //ui.button_widget_fluff_manipulator->setText(test.toString());
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
  u8 buffer;
  //if (DolphinComm::DolphinAccessor::updateRAMCache() == Common::MemOperationReturnCode::operationFailed) {
  if (!DolphinComm::DolphinAccessor::readFromRAM(0, &buffer, 1, false)) {
    ui.button_hook->setChecked(false);
    hook_unhook_attempt();
  }
}


void MainWindow::show_widget_spin() {
  if (!sms_spin_) {
    sms_spin_ = new Spin(nullptr);
    g_timer_16ms->connect(g_timer_16ms, SIGNAL(timeout()), sms_spin_, SLOT(update()));
  }
  sms_spin_->show();
  sms_spin_->raise();
  sms_spin_->activateWindow();
}

void MainWindow::show_widget_object_viewer() {
  if (!sms_object_viewer_) {
    sms_object_viewer_ = new ObjectViewer(nullptr);
    //connect(g_timer_100ms, &QTimer::timeout, sms_object_viewer_, QOverload<>::of(&ObjectViewer::on_update));
  }
  sms_object_viewer_->show();
  sms_object_viewer_->raise();
  sms_object_viewer_->activateWindow();
}

void MainWindow::show_widget_map_viewer() {
  if (!sms_map_viewer_) {
    sms_map_viewer_ = new MapViewer(nullptr);
  }
  sms_map_viewer_->show();
  sms_map_viewer_->raise();
  sms_map_viewer_->activateWindow();
}

void MainWindow::show_widget_fluff_manipulator() {
  if (!sms_data_)
    sms_data_ = new SMSData();
  if (!sms_fluff_manipulator_)
    sms_fluff_manipulator_ = new FluffManipulator(nullptr, sms_data_);
  sms_fluff_manipulator_->show();
  sms_fluff_manipulator_->raise();
  sms_fluff_manipulator_->activateWindow();
}

void MainWindow::show_widget_chuuhana() {
  if (!sms_chuuhana_main_) {
    sms_chuuhana_main_ = new ChuuHanaMain(nullptr);
  }
  sms_chuuhana_main_->show();
  sms_chuuhana_main_->raise();
  sms_chuuhana_main_->activateWindow();
}



void MainWindow::closeEvent(QCloseEvent* event) {
  if (sms_spin_)
    sms_spin_->close();
  if (sms_object_viewer_)
    sms_object_viewer_->close();
  if (sms_map_viewer_)
    sms_map_viewer_->close();
  if (sms_fluff_manipulator_)
    sms_fluff_manipulator_->close();
  if (sms_chuuhana_main_)
    sms_chuuhana_main_->close();

  if (test_main_window_)
    test_main_window_->close();

  event->accept();
}

void MainWindow::version_setup() {
  u8 version_value = memory::read_u8(0x80365ddd);
  if (version_value == 0x23) {  //JP 1.0
    // vtable‚Ì’l‚©‚çclass–¼‚ðƒ[ƒh(json)
    std::ifstream ifs("SMS/Resources/vtable_to_class_JP.json");
    nlohmann::json v_to_c;
    ifs >> v_to_c;
    g_vtable_to_class = v_to_c["Class"];
    return;
  }
  // TODO
  if (version_value == 0xa3) { // NA / KOR
    return;
  }
  if (version_value == 0x41) { // PAL
    return;
  }
  if (version_value == 0x80) { // JP 1.1
    return;
  }
  if (version_value == 0x4d) { // 3DAS
    return;
  }
}
