#include "ChuuHanaManipulator.h"

#include "../../Memory/Memory.h"
#include "RNGFunctions.h"

#include <QGroupBox>
#include <chrono>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>

ChuuHanaManipulator::ChuuHanaManipulator(QWidget* parent)
  : QWidget(parent) {
  rng_ = new ChuuHanaRNG();
  setWindowTitle(tr("ChuuHana Manipulator"));
  initialize_widgets();
  make_layouts();
  update();
  edited_seed_ = ram_seed_;
  edited_index_ = ram_index_;
  update_rng_textbox();
}

void ChuuHanaManipulator::initialize_widgets() {
  // update button
  btn_update_ = new QPushButton(tr("Update"), this);
  connect(btn_update_, &QPushButton::clicked, this, &ChuuHanaManipulator::update);

  // RNG seed, indexのラジオボタン
  rdb_read_from_ram_ = new QRadioButton(tr("Read RNG Seed from RAM"));
  rdb_edit_rng_seed_ = new QRadioButton(tr("RNG Seed: 0x"));
  rdb_edit_rng_index_ = new QRadioButton(tr("RNG Index: "));
  group_rdb_rng_ = new QButtonGroup(this);
  group_rdb_rng_->addButton(rdb_read_from_ram_, READ_FROM_RAM);
  group_rdb_rng_->addButton(rdb_edit_rng_seed_, EDIT_RNGSEED);
  group_rdb_rng_->addButton(rdb_edit_rng_index_, EDIT_RNGINDEX);
  rdb_read_from_ram_->setChecked(true);
  connect(group_rdb_rng_, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this, &ChuuHanaManipulator::update_rng_textbox);

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
  connect(txb_rng_seed_, &QLineEdit::textEdited, this, &ChuuHanaManipulator::on_rng_seed_changed);
  connect(txb_rng_index_, &QLineEdit::textEdited, this, &ChuuHanaManipulator::on_rng_index_changed);

  // 検索オプション
  // setGoal
  txb_range_from_ = new QLineEdit("29");
  txb_range_to_ = new QLineEdit("30");
  auto* val_m30_30 = new QRegExpValidator(QRegExp("-?[1-3]?[0-9](\\.?|\\.[0-9]?)"));
  txb_range_from_->setValidator(val_m30_30);
  txb_range_to_->setValidator(val_m30_30);
  // Node(willFall, CollidMove)
  lbl_nodes_ = new QLabel("Value: [0 .. 7]");
  group_chb_nodes_ = new QButtonGroup(this);
  group_chb_nodes_->setExclusive(true);
  chb_node_collid_move_ = new QCheckBox("CollidMove");
  chb_node_collid_move_->setChecked(true);
  cmb_node_count_ = new QComboBox();
  const QStringList str_cmb_node_count = { "0-6", "0-7" };
  cmb_node_count_->addItems(str_cmb_node_count);
  cmb_node_count_->setCurrentIndex(1);
  connect(cmb_node_count_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ChuuHanaManipulator::on_node_count_changed);
  chb_node_multiple_ = new QCheckBox("Multiple select");
  chb_node_multiple_->setChecked(false);
  connect(chb_node_multiple_, QOverload<int>::of(&QCheckBox::stateChanged), this, &ChuuHanaManipulator::on_chb_node_multiple_changed);
  // Radio Button: select RNG type from {Auto, setGoal, Node}
  rdb_type_auto_ = new QRadioButton("Auto: ");
  rdb_type_auto_->click();
  rdb_type_set_goal_ = new QRadioButton();
  rdb_type_node_ = new QRadioButton();
  group_rdb_rng_type_ = new QButtonGroup(this);
  group_rdb_rng_type_->addButton(rdb_type_auto_, TYPE_AUTO);
  group_rdb_rng_type_->addButton(rdb_type_set_goal_, TYPE_SET_GOAL);
  group_rdb_rng_type_->addButton(rdb_type_node_, TYPE_NODE);
  lbl_type_ = new QLabel("setGoal");

  // 確率
  lbl_probability_ = new QLabel();

  // seedの検索範囲
  txb_search_range_ = new QLineEdit("10000");
  txb_search_range_->setValidator(new QIntValidator(0, 10000000, this));
  txb_search_range_->setFixedWidth(100);

  // RNGテーブル
  model_rng_table_ = new ChuuHanaManipulatorModel(this, rng_);
  tbl_rng_table_ = new QTableView();
  tbl_rng_table_->setModel(model_rng_table_);
  tbl_rng_table_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tbl_rng_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  // 処理時間
  lbl_elapsed_time_ = new QLabel();
}

void ChuuHanaManipulator::make_layouts() {
  auto* lo_edit_rng_seed = new QHBoxLayout();
  lo_edit_rng_seed->setSpacing(0);
  lo_edit_rng_seed->addWidget(rdb_edit_rng_seed_);
  lo_edit_rng_seed->addWidget(txb_rng_seed_);

  auto* lo_edit_eng_index = new QHBoxLayout();
  lo_edit_eng_index->setSpacing(0);
  lo_edit_eng_index->addWidget(rdb_edit_rng_index_);
  lo_edit_eng_index->addWidget(txb_rng_index_);

  auto* lo_btn_group_edit = new QVBoxLayout();
  lo_btn_group_edit->addWidget(rdb_read_from_ram_);
  lo_btn_group_edit->addLayout(lo_edit_rng_seed);
  lo_btn_group_edit->addLayout(lo_edit_eng_index);

  auto* lo_range = new QHBoxLayout();
  auto* lbl_and = new QLabel("and");
  lo_range->addWidget(txb_range_from_);
  lo_range->addWidget(lbl_and);
  lo_range->addWidget(txb_range_to_);

  auto* lo_set_goal = new QVBoxLayout();
  auto* lbl_range = new QLabel("Value between: [-30.0, 30.0)");
  lo_set_goal->addWidget(lbl_range);
  lo_set_goal->addLayout(lo_range);
  auto* group_set_goal = new QGroupBox(tr("setGoal"));
  group_set_goal->setLayout(lo_set_goal);

  auto* lo_node = new QVBoxLayout();
  auto* lo_node_top = new QHBoxLayout();
  lo_node_top->addWidget(lbl_nodes_);
  lo_node_top->addWidget(cmb_node_count_);
  lo_node_top->addWidget(chb_node_collid_move_);
  lo_node_top->addStretch(0);
  lo_node_top->addWidget(chb_node_multiple_);
  lo_node->addLayout(lo_node_top);
  auto* lo_chb_nodes = new QHBoxLayout();
  QCheckBox* chb_nodes[8];
  for (s64 i = 0; i < 8; i++) {
    chb_nodes[i] = new QCheckBox(QString::number(i));
    group_chb_nodes_->addButton(chb_nodes[i], i);
    lo_chb_nodes->addWidget(chb_nodes[i]);
  }
  group_chb_nodes_->button(3)->click();
  lo_node->addLayout(lo_chb_nodes);
  auto* group_node = new QGroupBox("Node (willFall, CollidMove)");
  group_node->setLayout(lo_node);

  auto* lo_type_set_goal = new QHBoxLayout();
  lo_type_set_goal->addWidget(rdb_type_set_goal_);
  lo_type_set_goal->addWidget(group_set_goal);
  auto* lo_type_node = new QHBoxLayout();
  lo_type_node->addWidget(rdb_type_node_);
  lo_type_node->addWidget(group_node);
  auto* lo_top_left = new QVBoxLayout();
  lo_top_left->addWidget(rdb_type_auto_);
  lo_top_left->addLayout(lo_type_set_goal);
  lo_top_left->addLayout(lo_type_node);

  auto* lo_rng_information = new QHBoxLayout();
  lo_rng_information->addLayout(lo_btn_group_edit);
  lo_rng_information->addStretch(1);
  lo_rng_information->addLayout(lo_top_left);

  auto* lo_above_table = new QHBoxLayout();
  lo_above_table->addWidget(lbl_probability_);
  lo_above_table->addStretch(0);
  lo_above_table->addWidget(new QLabel("Search range"));
  lo_above_table->addWidget(txb_search_range_);

  auto* lo_bottom = new QHBoxLayout();
  lo_bottom->addWidget(lbl_elapsed_time_);

  auto* lo_main = new QVBoxLayout();
  lo_main->addWidget(btn_update_);
  lo_main->addLayout(lo_rng_information);
  lo_main->addLayout(lo_above_table);
  lo_main->addWidget(tbl_rng_table_);
  lo_main->addLayout(lo_bottom);

  setLayout(lo_main);
}

void ChuuHanaManipulator::update() {
  const auto start = std::chrono::steady_clock::now();

  if (group_rdb_rng_->checkedId() == READ_FROM_RAM) {
    if (DolphinComm::DolphinAccessor::getStatus() == DolphinComm::DolphinAccessor::DolphinStatus::hooked) {
      ram_seed_ = memory::read_u32(0x80408cf0);
    }
    rng_->set_seed(ram_seed_);
    ram_index_ = rng_->index_;
  } else {
    rng_->set_seed(edited_seed_);
  }
  update_rng_textbox();

  u32 search_range = 10000;
  search_range = txb_search_range_->text().toUInt();

  // TODO Auto選択時の動作の実装

  if (group_rdb_rng_type_->checkedId() == TYPE_AUTO || group_rdb_rng_type_->checkedId() == TYPE_SET_GOAL) {
    lbl_type_->setText("setGoal");
    const float min = txb_range_from_->text().toFloat();
    const float max = txb_range_to_->text().toFloat();
    rng_->search_rng_m30_30(min, max, search_range);
  } else if (group_rdb_rng_type_->checkedId() == TYPE_NODE) {
    std::vector<s32> checked;
    lbl_type_->setText("Node");
    for (QAbstractButton* chb : group_chb_nodes_->buttons()) {
      if (chb->isChecked())
        checked.push_back(group_chb_nodes_->id(chb));
    }
    const bool is_collided = (chb_node_collid_move_->checkState() == Qt::Checked);
    s32 rng_max = 6;
    if (cmb_node_count_->currentIndex() == 1) {
      rng_max = 7;
    }
    rng_->search_rng_int_array(0, rng_max, is_collided, checked, search_range);
  }

  model_rng_table_->update_list();

  lbl_probability_->setText("1/" + QString::number(rng_->probability_inv_));

  const auto end = std::chrono::steady_clock::now();
  const std::string elapsed_time = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + "ms";
  const std::string str_result_count = std::to_string(rng_->results_.size()) + " result(s) found";
  lbl_elapsed_time_->setText(QString::fromStdString(str_result_count + " (" + elapsed_time + ")"));

}

void ChuuHanaManipulator::update_rng_textbox() {
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

void ChuuHanaManipulator::on_rng_seed_changed() {
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

void ChuuHanaManipulator::on_rng_index_changed() {
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

void ChuuHanaManipulator::on_node_count_changed(s32 index) {
  if (index == 0) {
    group_chb_nodes_->button(7)->setDisabled(true);
    lbl_nodes_->setText("Value: [0 .. 6]");
  } else if (index == 1) {
    group_chb_nodes_->button(7)->setDisabled(false);
    lbl_nodes_->setText("Value: [0 .. 7]");
  }
}

void ChuuHanaManipulator::on_chb_node_multiple_changed(s32 state) {
  if (state == Qt::Unchecked) {
    group_chb_nodes_->setExclusive(true);
  } else if (state == Qt::Checked) {
    group_chb_nodes_->setExclusive(false);
  }
}
