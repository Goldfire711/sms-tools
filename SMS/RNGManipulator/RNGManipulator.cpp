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
  search();
}

void RNGManipulator::initialize_widgets() {
  // search button
  btn_search_ = new QPushButton(tr("Search"), this);
  connect(btn_search_, &QPushButton::clicked, this, &RNGManipulator::search);

  // Radio Button to select initial seed for search
  rdb_rng_ = new RNGRadioButton(this);

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

  auto* lo_rdb_rng = new QVBoxLayout();
  lo_rdb_rng->addWidget(rdb_rng_);
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

void RNGManipulator::search() const {
  const auto start = std::chrono::steady_clock::now();

  rng_->set_seed(rdb_rng_->get_seed());

  const u32 search_range = txb_search_range_->text().toUInt();
  const float search_min = txb_search_from_->text().toFloat();
  const float search_max = txb_search_to_->text().toFloat();
  const float rng_min = txb_rng_from_->text().toFloat();
  const float rng_max = txb_rng_to_->text().toFloat();
  rng_->search_rng(search_min, search_max, rng_min, rng_max, search_range);

  model_tbl_rng_->update_list();

  lbl_probability_->setText("1/" + QString::number(rng_->probability_inv_));

  const auto end = std::chrono::steady_clock::now();
  const std::string elapsed_time = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + "ms";
  const std::string str_result_count = std::to_string(rng_->results_.size()) + " result(s) found";
  lbl_elapsed_time_->setText(QString::fromStdString(str_result_count + " (" + elapsed_time + ")"));
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