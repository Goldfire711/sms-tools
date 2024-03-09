#include "RNGViewer.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include "RNGFunctions.h"
#include "../../Memory/Memory.h"

extern QTimer* g_timer_100ms;
using namespace memory;
//test
RNGViewer::RNGViewer(QWidget* parent)
  : QWidget(parent) {
  // RAM RNG
  lbl_seed_ = new QLabel("RAM Seed: 0x");
  lbl_index_ = new QLabel("RAM Index: ");
  txb_seed_ = new QLineEdit("0");
  txb_index_ = new QLineEdit("0");
  txb_seed_->setFixedWidth(146);
  txb_index_->setFixedWidth(146);
  txb_seed_->setReadOnly(true);
  txb_index_->setReadOnly(true);
  txb_seed_->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0); border: none }");
  txb_index_->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0); border: none }");

  // Edit RNG
  lbl_edit_seed_ = new QLabel("Edit Seed: 0x");
  txb_edit_seed_ = new QLineEdit("0");
  txb_edit_seed_->setFixedWidth(146);
  const auto* val_hex_32 = new QRegExpValidator(QRegExp("[0-9a-fA-F]{1,8}"));
  txb_edit_seed_->setValidator(val_hex_32);
  connect(txb_edit_seed_, &QLineEdit::textEdited, this, &RNGViewer::on_edit_seed_changed);
  lbl_edit_index_ = new QLabel("Edit Index: ");
  spb_edit_index_ = new U32RNGSpinBox(this);
  spb_edit_index_->setFixedWidth(146);
  connect(spb_edit_index_, &QSpinBox::textChanged, this, &RNGViewer::on_edit_index_changed);
  lbl_index_diff_ = new QLabel("Index Diff: ");
  lbl_index_diff2_ = new QLabel("+0");
  lbl_index_diff2_->setFixedWidth(146);
  lbl_index_diff2_->setTextInteractionFlags(Qt::TextSelectableByMouse);

  // Edit buttons
  chb_edit_ = new QCheckBox("Edit");
  connect(chb_edit_, QOverload<int>::of(&QCheckBox::stateChanged), this, &RNGViewer::on_chb_edit_changed);
  btn_read_ = new QPushButton(QString::fromLocal8Bit("«Read"));
  btn_read_->setDisabled(true);
  btn_read_->setFixedWidth(73);
  connect(btn_read_, &QPushButton::clicked, this, [=] {
    spb_edit_index_->setValueU32(index_);
  });
  btn_write_ = new QPushButton(QString::fromLocal8Bit("ªWrite"));
  btn_write_->setDisabled(true);
  btn_write_->setFixedWidth(73);
  connect(btn_write_, &QPushButton::clicked, this, [=] {
    if (DolphinComm::DolphinAccessor::getStatus() == DolphinComm::DolphinAccessor::DolphinStatus::hooked)
      write_u32(0x80408cf0, edit_seed_);
  });

  auto* lo_seed = new QHBoxLayout();
  lo_seed->setSpacing(0);
  lo_seed->addWidget(lbl_seed_);
  lo_seed->addWidget(txb_seed_);

  auto* lo_index = new QHBoxLayout();
  lo_index->setSpacing(0);
  lo_index->addWidget(lbl_index_);
  lo_index->addWidget(txb_index_);

  auto* lo_middle = new QHBoxLayout();
  lo_middle->addWidget(chb_edit_);
  lo_middle->addStretch(0);
  lo_middle->addWidget(btn_read_);
  lo_middle->addWidget(btn_write_);

  auto* lo_edit_seed = new QHBoxLayout();
  lo_edit_seed->setSpacing(0);
  lo_edit_seed->addWidget(lbl_edit_seed_);
  lo_edit_seed->addWidget(txb_edit_seed_);

  auto* lo_edit_index = new QHBoxLayout();
  lo_edit_index->setSpacing(0);
  lo_edit_index->addWidget(lbl_edit_index_);
  lo_edit_index->addWidget(spb_edit_index_);

  auto* lo_index_diff = new QHBoxLayout();
  lo_index_diff->setSpacing(0);
  lo_index_diff->addWidget(lbl_index_diff_);
  lo_index_diff->addWidget(lbl_index_diff2_);

  auto* lo_main = new QVBoxLayout();
  lo_main->addLayout(lo_seed);
  lo_main->addLayout(lo_index);
  lo_main->addLayout(lo_middle);
  lo_main->addLayout(lo_edit_seed);
  lo_main->addLayout(lo_edit_index);
  lo_main->addLayout(lo_index_diff);

  lo_main->setSizeConstraint(QLayout::SetMinimumSize);
  lo_main->setMargin(0);
  lo_main->setSpacing(0);

  setLayout(lo_main);
  
  connect(g_timer_100ms, &QTimer::timeout, this, QOverload<>::of(&RNGViewer::update_ram_rng));
  on_chb_edit_changed(Qt::Unchecked);
}

void RNGViewer::update_ram_rng() {
  if (DolphinComm::DolphinAccessor::getStatus() == DolphinComm::DolphinAccessor::DolphinStatus::hooked) {
    if (const u32 seed = read_u32(0x80408cf0); seed != seed_) {
      seed_ = seed;
      index_ = rng::seed_to_index(seed);

      txb_seed_->setText(QString::number(seed, 16).toUpper());
      txb_index_->setText(QString::number(index_));
      const s64 diff = static_cast<s64>(edit_index_) - static_cast<s64>(index_);
      lbl_index_diff2_->setText((diff < 0 ? "" : "+") + QString::number(diff));
    }
  }
  
  if (seed_ == edit_seed_) {
    lbl_edit_seed_->setStyleSheet("");
    lbl_edit_index_->setStyleSheet("");
    lbl_index_diff_->setStyleSheet("");
    btn_read_->setDisabled(true);
    btn_write_->setDisabled(true);
  } else {
    lbl_edit_seed_->setStyleSheet("QLabel { color : Salmon; }");
    lbl_edit_index_->setStyleSheet("QLabel { color : Salmon; }");
    lbl_index_diff_->setStyleSheet("QLabel { color : Salmon; }");
    btn_read_->setDisabled(false);
    btn_write_->setDisabled(false);
  }
}

void RNGViewer::on_edit_seed_changed() {
  if (!txb_edit_seed_->text().isEmpty()) {
    edit_seed_ = txb_edit_seed_->text().toUInt(nullptr, 16);
    edit_index_ = rng::seed_to_index(edit_seed_);
  }
  spb_edit_index_->setValueU32(edit_index_);
  const s64 diff = static_cast<s64>(edit_index_) - static_cast<s64>(index_);
  lbl_index_diff2_->setText((diff < 0 ? "" : "+") + QString::number(diff));
}

void RNGViewer::on_edit_index_changed() {
  edit_index_ = spb_edit_index_->valueU32();
  edit_seed_ = rng::index_to_seed(edit_index_);
  txb_edit_seed_->setText(QString::number(edit_seed_, 16).toUpper());
  const s64 diff = static_cast<s64>(edit_index_) - static_cast<s64>(index_);
  lbl_index_diff2_->setText((diff < 0 ? "" : "+") + QString::number(diff));
}

void RNGViewer::on_chb_edit_changed(s32 state) const {
  if (state == Qt::Checked) {
    btn_read_->setVisible(true);
    btn_write_->setVisible(true);
    lbl_edit_seed_->setVisible(true);
    txb_edit_seed_->setVisible(true);
    lbl_edit_index_->setVisible(true);
    spb_edit_index_->setVisible(true);
    lbl_index_diff_->setVisible(true);
    lbl_index_diff2_->setVisible(true);
  } else {
    btn_read_->setVisible(false);
    btn_write_->setVisible(false);
    lbl_edit_seed_->setVisible(false);
    txb_edit_seed_->setVisible(false);
    lbl_edit_index_->setVisible(false);
    spb_edit_index_->setVisible(false);
    lbl_index_diff_->setVisible(false);
    lbl_index_diff2_->setVisible(false);
  }
}