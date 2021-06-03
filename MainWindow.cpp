#include "MainWindow.h"

#include "DolphinProcess/DolphinAccessor.h"
#include "Common/CommonUtils.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent) {
	ui_.setupUi(this);
  connect(ui_.button_hook_, &QPushButton::clicked, this, &MainWindow::OnButtonHookClicked);
	DolphinComm::DolphinAccessor::init();
  OnButtonHookClicked();
}

void MainWindow::OnButtonHookClicked() {
  if (ui_.button_hook_->isChecked()) {
    DolphinComm::DolphinAccessor::hook();
  } else {
    DolphinComm::DolphinAccessor::unHook();
  }

  switch (DolphinComm::DolphinAccessor::getStatus()) {
  case DolphinComm::DolphinAccessor::DolphinStatus::hooked: {
    ui_.label_dolphin_status_->setText(
      tr("Start address: ") +
      QString::number(DolphinComm::DolphinAccessor::getEmuRAMAddressStart(), 16).toUpper());
    ui_.button_hook_->setText(tr("Unhook"));
    ui_.button_hook_->setChecked(true);
    ui_.label->setText(QString::fromStdString(GetGameID()));
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::notRunning: {
    ui_.label_dolphin_status_->setText(tr("Dolphin is not running"));
    ui_.button_hook_->setText(tr("Hook"));
    ui_.button_hook_->setChecked(false);
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::noEmu: {
    ui_.label_dolphin_status_->setText(tr("SMS is not running"));
    ui_.button_hook_->setText(tr("Hook"));
    ui_.button_hook_->setChecked(false);
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::unHooked: {
    ui_.label_dolphin_status_->setText(tr("Unhooked"));
    ui_.button_hook_->setText(tr("Hook"));
    ui_.button_hook_->setChecked(false);
    break;
  }
  }
}

std::string MainWindow::GetGameID() {
  char* memory = new char[7];
  DolphinComm::DolphinAccessor::readFromRAM(Common::dolphinAddrToOffset(0x80000000), memory, 7, false);
  //return Common::formatMemoryToString(memory, Common::MemType::type_string, 7, Common::MemBase::base_decimal, true);
  std::string s = memory;
  delete[] memory;
  return s;
}
