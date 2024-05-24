#include "RNGManipulator.h"

#include "../../Memory/Memory.h"
#include "RNGFunctions.h"

#include <QGroupBox>
#include <chrono>
#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QClipboard>
#include <QMenu>
#include <QShortcut>
#include <QKeySequence>

using namespace memory;

RNGManipulator::RNGManipulator(QWidget* parent)
  : QWidget(parent) {
  rng_ = new RNG();
  setWindowTitle(tr("RNG Manipulator"));
  initialize_widgets();
  make_layouts();
  update();
  edited_seed_ = ram_seed_;
  edited_index_ = ram_index_;
  update_rng_textbox();
}

void RNGManipulator::initialize_widgets() {
  // search button
  btn_search_ = new QPushButton(tr("Search"), this);
  connect(btn_search_, &QPushButton::clicked, this, &RNGManipulator::update);

  // RNG seed, indexのラジオボタン
  rdb_read_from_ram_ = new QRadioButton(tr("Read RNG Seed from RAM"));
  rdb_edit_rng_seed_ = new QRadioButton(tr("RNG Seed: 0x"));
  rdb_edit_rng_index_ = new QRadioButton(tr("RNG Index: "));
  group_rdb_rng_ = new QButtonGroup(this);
  group_rdb_rng_->addButton(rdb_read_from_ram_, READ_FROM_RAM);
  group_rdb_rng_->addButton(rdb_edit_rng_seed_, EDIT_RNGSEED);
  group_rdb_rng_->addButton(rdb_edit_rng_index_, EDIT_RNGINDEX);
  rdb_read_from_ram_->setChecked(true);
  connect(group_rdb_rng_, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this, &RNGManipulator::update_rng_textbox);

  // seed, indexのLineEdit
  txb_rng_seed_ = new QLineEdit("FFFFFFFF");
  txb_rng_index_ = new QLineEdit("4294967295");
  txb_rng_seed_->setFixedWidth(120);
  txb_rng_index_->setFixedWidth(120);
  auto* val_hex_32 = new QRegExpValidator(QRegExp("[0-9a-fA-F]{1,8}"), this);
  const QRegExp reg_uint(
    R"(^(0|(\+)?[1-9]{1}[0-9]{0,8}|(\+)?[1-3]{1}[0-9]{1,9}|(\+)?[4]{1}([0-1]{1}[0-9]{8}|[2]{1}([0-8]{1}[0-9]{7}|[9]{1}([0-3]{1}[0-9]{6}|[4]{1}([0-8]{1}[0-9]{5}|[9]{1}([0-5]{1}[0-9]{4}|[6]{1}([0-6]{1}[0-9]{3}|[7]{1}([0-1]{1}[0-9]{2}|[2]{1}([0-8]{1}[0-9]{1}|[9]{1}[0-5]{1})))))))))$)");
  auto* val_uint = new QRegExpValidator(reg_uint, this);
  txb_rng_seed_->setValidator(val_hex_32);
  txb_rng_index_->setValidator(val_uint);
  connect(txb_rng_seed_, &QLineEdit::textEdited, this, &RNGManipulator::on_rng_seed_changed);
  connect(txb_rng_index_, &QLineEdit::textEdited, this, &RNGManipulator::on_rng_index_changed);

  // 検索オプション
  auto* val_float = new QRegExpValidator(QRegExp(R"(-?\d*(\.\d+)?)"));
  txb_rng_from_ = new QLineEdit("0");
  txb_rng_to_ = new QLineEdit("1");
  txb_rng_from_->setValidator(val_float);
  txb_rng_to_->setValidator(val_float);
  txb_rng_from_->setFixedWidth(60);
  txb_rng_to_->setFixedWidth(60);
  txb_search_from_ = new QLineEdit("0");
  txb_search_to_ = new QLineEdit("0.01");
  txb_search_from_->setValidator(val_float);
  txb_search_to_->setValidator(val_float);
  txb_search_from_->setFixedWidth(60);
  txb_search_to_->setFixedWidth(60);

  // GroupBoxes
  group_setting_ = new QGroupBox(tr("Settings"));

  // 確率
  lbl_probability_ = new QLabel();

  // seedの検索範囲
  txb_search_range_ = new QLineEdit("10000");
  txb_search_range_->setValidator(new QRegExpValidator(QRegExp("\\d{1,10}")));
  txb_search_range_->setFixedWidth(100);

  // RNGテーブル
  model_tbl_rng_ = new RNGManipulatorModel(this, rng_);
  tbl_rng_ = new QTableView(this);
  tbl_rng_->setModel(model_tbl_rng_);
  tbl_rng_->setSelectionMode(QAbstractItemView::ContiguousSelection);
  tbl_rng_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  tbl_rng_->setContextMenuPolicy(Qt::CustomContextMenu);
  delegate_ = new RNGManipulatorDelegate(this);
  tbl_rng_->setItemDelegateForColumn(RNGManipulatorModel::COL_7A5B_INDEX, delegate_);
  connect(tbl_rng_, &QWidget::customContextMenuRequested, this, &RNGManipulator::on_context_menu_requested);

  // 処理時間
  lbl_elapsed_time_ = new QLabel();

  // ショートカットキー (Ctrl + C)
  const auto* copy_shortcut = new QShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_C), tbl_rng_);
  connect(copy_shortcut, &QShortcut::activated, this, &RNGManipulator::copy_selection);

  // RNG Viewer
  rng_viewer_ = new RNGViewer(this);
}

void RNGManipulator::make_layouts() {
  auto* lo_rng_viewer = new QVBoxLayout();
  lo_rng_viewer->addWidget(rng_viewer_);
  auto* group_rng_viewer = new QGroupBox("RAM RNG");
  group_rng_viewer->setLayout(lo_rng_viewer);

  auto* lo_edit_rng_seed = new QHBoxLayout();
  lo_edit_rng_seed->setSpacing(0);
  lo_edit_rng_seed->addWidget(rdb_edit_rng_seed_);
  lo_edit_rng_seed->addWidget(txb_rng_seed_);

  auto* lo_edit_rng_index = new QHBoxLayout();
  lo_edit_rng_index->setSpacing(0);
  lo_edit_rng_index->addWidget(rdb_edit_rng_index_);
  lo_edit_rng_index->addWidget(txb_rng_index_);

  auto* lo_rdb_rng = new QVBoxLayout();
  lo_rdb_rng->addWidget(rdb_read_from_ram_);
  lo_rdb_rng->addLayout(lo_edit_rng_seed);
  lo_rdb_rng->addLayout(lo_edit_rng_index);
  auto* group_rdb_rng = new QGroupBox("Search RNG");
  group_rdb_rng->setLayout(lo_rdb_rng);

  auto* lo_rng = new QVBoxLayout();
  lo_rng->addWidget(group_rng_viewer);
  lo_rng->addWidget(group_rdb_rng);

  auto* lo_rng_range = new QHBoxLayout();
  auto* lbl_rng_range = new QLabel("RNG range:");
  auto* lbl_rng_hyphen = new QLabel("-");
  lo_rng_range->addWidget(lbl_rng_range);
  lo_rng_range->addStretch(0);
  lo_rng_range->addWidget(txb_rng_from_);
  lo_rng_range->addWidget(lbl_rng_hyphen);
  lo_rng_range->addWidget(txb_rng_to_);

  auto* lo_search_range = new QHBoxLayout();
  auto* lbl_search_range = new QLabel("Search range:");
  auto* lbl_search_hyphen = new QLabel("-");
  lo_search_range->addWidget(lbl_search_range);
  lo_search_range->addStretch(0);
  lo_search_range->addWidget(txb_search_from_);
  lo_search_range->addWidget(lbl_search_hyphen);
  lo_search_range->addWidget(txb_search_to_);

  auto* lo_setting = new QVBoxLayout();
  lo_setting->addLayout(lo_rng_range);
  lo_setting->addLayout(lo_search_range);
  group_setting_->setLayout(lo_setting);

  auto* lo_rng_information = new QHBoxLayout();
  lo_rng_information->addLayout(lo_rng);
  lo_rng_information->addStretch(1);
  lo_rng_information->addWidget(group_setting_);

  auto* lo_above_table = new QHBoxLayout();
  lo_above_table->addWidget(lbl_probability_);
  lo_above_table->addStretch(0);
  lo_above_table->addWidget(new QLabel("Search range"));
  lo_above_table->addWidget(txb_search_range_);

  auto* lo_bottom = new QHBoxLayout();
  lo_bottom->addWidget(lbl_elapsed_time_);

  auto* lo_main = new QVBoxLayout();
  lo_main->addWidget(btn_search_);
  lo_main->addLayout(lo_rng_information);
  lo_main->addLayout(lo_above_table);
  lo_main->addWidget(tbl_rng_);
  lo_main->addLayout(lo_bottom);

  setLayout(lo_main);
}

void RNGManipulator::update() {
  const auto start = std::chrono::steady_clock::now();

  if (group_rdb_rng_->checkedId() == READ_FROM_RAM) {
    if (DolphinComm::DolphinAccessor::getStatus() == DolphinComm::DolphinAccessor::DolphinStatus::hooked) {
      ram_seed_ = read_u32(0x80408cf0);
    }
    rng_->set_seed(ram_seed_);
    ram_index_ = rng_->index_;
  } else {
    rng_->set_seed(edited_seed_);
  }
  update_rng_textbox();

  const u32 search_range = txb_search_range_->text().toUInt();
  const float search_min = txb_search_from_->text().toFloat();
  const float search_max = txb_search_to_->text().toFloat();
  const float rng_min = txb_rng_from_->text().toFloat();
  const float rng_max = txb_rng_to_->text().toFloat();
  qDebug() << search_min << "\n" << search_max << "\n" << rng_min << "\n" << rng_max << "\n";
  rng_->search_rng(search_min, search_max, rng_min, rng_max, search_range);

  model_tbl_rng_->update_list();

  lbl_probability_->setText("1/" + QString::number(rng_->probability_inv_));

  const auto end = std::chrono::steady_clock::now();
  const std::string elapsed_time = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + "ms";
  const std::string str_result_count = std::to_string(rng_->results_.size()) + " result(s) found";
  lbl_elapsed_time_->setText(QString::fromStdString(str_result_count + " (" + elapsed_time + ")"));
}

void RNGManipulator::update_rng_textbox() {
  switch (group_rdb_rng_->checkedId()) {
  case READ_FROM_RAM:
    txb_rng_seed_->setText(QString::number(ram_seed_, 16).toUpper());
    txb_rng_seed_->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0); border: none }");
    txb_rng_seed_->setReadOnly(true);
    txb_rng_index_->setText(QString::number(ram_index_));
    txb_rng_index_->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0); border: none }");
    txb_rng_index_->setReadOnly(true);
    break;
  case EDIT_RNGSEED:
    txb_rng_seed_->setText(QString::number(edited_seed_, 16).toUpper());
    txb_rng_seed_->setStyleSheet("");
    txb_rng_seed_->setReadOnly(false);
    txb_rng_index_->setText(QString::number(edited_index_));
    txb_rng_index_->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0); border: none }");
    txb_rng_index_->setReadOnly(true);
    break;
  case EDIT_RNGINDEX:
    txb_rng_seed_->setText(QString::number(edited_seed_, 16).toUpper());
    txb_rng_seed_->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0); border: none }");
    txb_rng_seed_->setReadOnly(true);
    txb_rng_index_->setText(QString::number(edited_index_));
    txb_rng_index_->setStyleSheet("");
    txb_rng_index_->setReadOnly(false);
    break;
  default:
    break;
  }
}

void RNGManipulator::on_rng_seed_changed() {
  u32 seed = 0;
  u32 index = 0;
  if (!txb_rng_seed_->text().isEmpty()) {
    std::stringstream ss(txb_rng_seed_->text().toStdString());
    ss >> std::hex >> seed;
    index = rng::seed_to_index(seed);
  }
  edited_seed_ = seed;
  edited_index_ = index;
  txb_rng_index_->setText(QString::number(index));
}

void RNGManipulator::on_rng_index_changed() {
  u32 seed = 0;
  u32 index = 0;
  if (!txb_rng_index_->text().isEmpty()) {
    std::stringstream ss(txb_rng_index_->text().toStdString());
    ss >> std::dec >> index;
    seed = rng::index_to_seed(index);
  }
  edited_seed_ = seed;
  edited_index_ = index;
  txb_rng_seed_->setText(QString::number(seed, 16).toUpper());
}

void RNGManipulator::on_context_menu_requested(const QPoint& pos) {
  QModelIndex index = tbl_rng_->indexAt(pos);
  QModelIndexList selection = tbl_rng_->selectionModel()->selectedIndexes();
  copy_selection();
  auto* menu = new QMenu(this);
  auto* copy = new QAction("Copy", this);
  connect(copy, &QAction::triggered, this, &RNGManipulator::copy_selection);
  menu->addAction(copy);
  if (selection.count() == 1 && index.column() == 3) {
    auto* write_to_ram = new QAction("Write to RAM", this);
    connect(write_to_ram, &QAction::triggered, this, [=] {
      const u32 rng = index.data().toString().toUInt(nullptr, 16);
      write_u32(0x80408cf0, rng);
      });
    menu->addAction(write_to_ram);
  }

  if (selection.count() != 0) {
    menu->popup(tbl_rng_->viewport()->mapToGlobal(pos));
  }
}

void RNGManipulator::copy_selection() const {
  QString str;
  auto selection = tbl_rng_->selectionModel()->selectedIndexes();
  if (selection.isEmpty())
    return;
  for (s32 i = 0; i < selection.count(); i++) {
    const auto* index = &selection[i];
    if (index->column() != selection.last().column()) {
      QTextStream(&str) << index->data().toString() << "\t";
    } else {
      if (index->row() != selection.last().row()) {
        QTextStream(&str) << index->data().toString() << "\n";
      } else {
        QTextStream(&str) << index->data().toString();
      }
    }
  }

  auto* clipboard = QApplication::clipboard();
  clipboard->setText(str);
}