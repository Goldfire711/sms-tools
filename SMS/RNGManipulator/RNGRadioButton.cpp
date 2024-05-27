#include "RNGRadioButton.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPushButton>

#include "Memory/Memory.h"
#include "SMS/ChuuHana/RNGFunctions.h"

RNGRadioButton::RNGRadioButton(QWidget* parent) : QWidget(parent) {
  // initialize widgets
  rdb_read_from_ram_ = new QRadioButton(tr("Read RNG Seed from RAM"));
  rdb_edit_rng_ = new QRadioButton();

  grp_rdb_ = new QButtonGroup(this);
  grp_rdb_->addButton(rdb_read_from_ram_, READ_FROM_RAM);
  grp_rdb_->addButton(rdb_edit_rng_, EDIT_RNG);
  rdb_read_from_ram_->setChecked(true);

  connect(grp_rdb_, &QButtonGroup::idToggled, this, &RNGRadioButton::on_rdb_clicked);

  txb_rng_seed_ = new QLineEdit("0");
  txb_rng_seed_->setFixedWidth(110);
  const auto* val_hex_32 = new QRegExpValidator(QRegExp("[0-9a-fA-F]{1,8}"), this);
  txb_rng_seed_->setValidator(val_hex_32);

  spb_rng_index_ = new U32RNGSpinBox(this);
  spb_rng_index_->setFixedWidth(110);

  connect(txb_rng_seed_, &QLineEdit::textEdited, this, &RNGRadioButton::on_rng_seed_changed);
  connect(spb_rng_index_, &QSpinBox::textChanged, this, &RNGRadioButton::on_rng_index_changed);

  auto* lbl_edit_rng_seed = new ClickableLabel("RNG Seed: 0x");
  connect(lbl_edit_rng_seed, &ClickableLabel::clicked, this, [this] {
    rdb_edit_rng_->setChecked(true);
    });

  auto* lbl_edit_rng_index = new ClickableLabel("RNG Index: ");
  connect(lbl_edit_rng_index, &ClickableLabel::clicked, this, [this] {
    rdb_edit_rng_->setChecked(true);
    });

  // make layouts
  auto* lo_edit_rng_seed = new QHBoxLayout;
  lo_edit_rng_seed->setSpacing(0);
  lo_edit_rng_seed->addWidget(lbl_edit_rng_seed);
  lo_edit_rng_seed->addWidget(txb_rng_seed_);

  auto* lo_edit_rng_index = new QHBoxLayout;
  lo_edit_rng_index->setSpacing(0);
  lo_edit_rng_index->addWidget(lbl_edit_rng_index);
  lo_edit_rng_index->addWidget(spb_rng_index_);

  auto* lo_edit_rng_right = new QVBoxLayout;
  lo_edit_rng_right->setSpacing(0);
  lo_edit_rng_right->addLayout(lo_edit_rng_seed);
  lo_edit_rng_right->addLayout(lo_edit_rng_index);

  auto* lo_edit_rng = new QHBoxLayout;
  lo_edit_rng->addWidget(rdb_edit_rng_);
  lo_edit_rng->addLayout(lo_edit_rng_right);

  auto* main = new QVBoxLayout;
  main->addWidget(rdb_read_from_ram_);
  main->addLayout(lo_edit_rng);

  setLayout(main);

  // initialize edit_seed, edit_index
  get_seed();
  edited_seed_ = ram_seed_;
  edited_index_ = ram_index_;
}

void RNGRadioButton::on_rdb_clicked(const s32 id) const {
  switch (id) {
  case READ_FROM_RAM:
    txb_rng_seed_->setText(QString::number(ram_seed_, 16).toUpper());
    txb_rng_seed_->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0); border: none }");
    txb_rng_seed_->setReadOnly(true);
    spb_rng_index_->setValueU32(ram_index_);
    spb_rng_index_->setStyleSheet(
      "QSpinBox { background-color: rgba(0, 0, 0, 0); border: none }"
         "QSpinBox::up-button { width: 0; height: 0; border: none; }"
         "QSpinBox::down-button {width: 0; height: 0; border: none; }"
    );
    spb_rng_index_->setReadOnly(true);
    break;
  case EDIT_RNG:
    txb_rng_seed_->setText(QString::number(edited_seed_, 16).toUpper());
    txb_rng_seed_->setStyleSheet("");
    txb_rng_seed_->setReadOnly(false);
    spb_rng_index_->setValueU32(edited_index_);
    spb_rng_index_->setStyleSheet("");
    spb_rng_index_->setReadOnly(false);
    break;
  default:
    break;
  }
}

void RNGRadioButton::on_rng_seed_changed(const QString& str_seed) {
  if (rdb_read_from_ram_->isChecked())
    return;

  u32 seed = 0;
  u32 index = 0;
  if (!str_seed.isEmpty()) {
    seed = str_seed.toUInt();
    index = rng::seed_to_index(seed);
  }
  edited_seed_ = seed;
  edited_index_ = index;

  spb_rng_index_->setValueU32(index);
}

void RNGRadioButton::on_rng_index_changed() {
  if (rdb_read_from_ram_->isChecked())
    return;

  edited_index_ = spb_rng_index_->valueU32();
  edited_seed_ = rng::index_to_seed(edited_index_);

  txb_rng_seed_->setText(QString::number(edited_seed_, 16).toUpper());
}

u32 RNGRadioButton::get_seed() {
  if (grp_rdb_->checkedId() == READ_FROM_RAM) {
    if (DolphinComm::DolphinAccessor::getStatus() == DolphinComm::DolphinAccessor::DolphinStatus::hooked) {
      ram_seed_ = memory::read_u32(0x80408cf0);
      ram_index_ = rng::seed_to_index(ram_seed_);
    }
    on_rdb_clicked(READ_FROM_RAM);
    return ram_seed_;
  }
  return edited_seed_;
}
