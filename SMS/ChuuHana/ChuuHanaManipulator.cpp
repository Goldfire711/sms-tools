#include "ChuuHanaManipulator.h"
#include "ChuuHanaManipulator.h"

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
  // search button
  btn_search_ = new QPushButton(tr("Search"), this);
  connect(btn_search_, &QPushButton::clicked, this, &ChuuHanaManipulator::update);

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
  lbl_nodes_ = new QLabel("Value: ");
  group_chb_nodes_ = new QButtonGroup(this);
  group_chb_nodes_->setExclusive(true);
  chb_node_collid_move_ = new QCheckBox("CollidMove");
  chb_node_collid_move_->setChecked(true);
  chb_node_collid_move_->setDisabled(true);
  cmb_node_range_ = new QComboBox();
  const QStringList str_cmb_node_range = { "0-6", "0-7" };
  cmb_node_range_->addItems(str_cmb_node_range);
  cmb_node_range_->setCurrentIndex(ZERO_TO_SEVEN);
  cmb_node_range_->setDisabled(true);
  connect(cmb_node_range_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ChuuHanaManipulator::on_node_count_changed);
  chb_node_multiple_ = new QCheckBox("Multiple select");
  chb_node_multiple_->setChecked(false);
  connect(chb_node_multiple_, QOverload<int>::of(&QCheckBox::stateChanged), this, &ChuuHanaManipulator::on_chb_node_multiple_changed);
  // Radio Button: select RNG type from {Auto, setGoal, Node}
  rdb_type_auto_ = new QRadioButton("Auto: ");
  rdb_type_auto_->click();
  btn_auto_left_ = new QPushButton();
  btn_auto_left_->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
  btn_auto_left_->setDisabled(true);
  btn_auto_right_ = new QPushButton();
  btn_auto_right_->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
  btn_auto_right_->setDisabled(true);
  btn_auto_left_->setFixedWidth(30);
  btn_auto_right_->setFixedWidth(30);
  connect(btn_auto_left_, &QPushButton::clicked, this, &ChuuHanaManipulator::on_button_left_clicked);
  connect(btn_auto_right_, &QPushButton::clicked, this, &ChuuHanaManipulator::on_button_right_clicked);
  rdb_type_set_goal_ = new QRadioButton();
  rdb_type_node_ = new QRadioButton();
  group_rdb_rng_type_ = new QButtonGroup(this);
  group_rdb_rng_type_->addButton(rdb_type_auto_, TYPE_AUTO);
  group_rdb_rng_type_->addButton(rdb_type_set_goal_, TYPE_SET_GOAL);
  group_rdb_rng_type_->addButton(rdb_type_node_, TYPE_NODE);
  connect(group_rdb_rng_type_, &QButtonGroup::idClicked, this, &ChuuHanaManipulator::on_rng_type_changed);
  lbl_type_ = new QLabel("setGoal");
  // GroupBoxes
  group_set_goal_ = new QGroupBox(tr("setGoal"));
  group_set_goal_->setContextMenuPolicy(Qt::ActionsContextMenu);
  auto* copy_bp_set_goal = new QAction("Copy BP (setGoal: 0x802C02F8)", this);
  connect(copy_bp_set_goal, &QAction::triggered, this, [=] {
    QApplication::clipboard()->setText("802C02F8");
    });
  group_set_goal_->addAction(copy_bp_set_goal);
  group_node_ = new QGroupBox("willFall, isCollidMove");
  group_node_->setContextMenuPolicy(Qt::ActionsContextMenu);
  auto* copy_bp_will_fall = new QAction("Copy BP (willFall: 0x802C050C)", this);
  auto* copy_bp_is_collid_move = new QAction("Copy BP (isCollidMove: 0x802C13D4)", this);
  connect(copy_bp_will_fall, &QAction::triggered, this, [=] {
    QApplication::clipboard()->setText("802C050C");
    });
  connect(copy_bp_is_collid_move, &QAction::triggered, this, [=] {
    QApplication::clipboard()->setText("802C13D4");
    });
  group_node_->addActions({ copy_bp_will_fall, copy_bp_is_collid_move });

  // 確率
  lbl_probability_ = new QLabel();

  // seedの検索範囲
  txb_search_range_ = new QLineEdit("10000");
  txb_search_range_->setValidator(new QIntValidator(0, 10000000, this));
  txb_search_range_->setFixedWidth(100);

  // RNGテーブル
  model_tbl_rng_ = new ChuuHanaManipulatorModel(this, rng_);
  tbl_rng_ = new QTableView(this);
  tbl_rng_->setModel(model_tbl_rng_);
  tbl_rng_->setSelectionMode(QAbstractItemView::ContiguousSelection);
  tbl_rng_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  tbl_rng_->setContextMenuPolicy(Qt::CustomContextMenu);
  delegate_ = new ChuuHanaManipulatorDelegate(this);
  tbl_rng_->setItemDelegateForColumn(ChuuHanaManipulatorModel::COL_7A5B_INDEX, delegate_);
  connect(tbl_rng_, &QWidget::customContextMenuRequested, this, &ChuuHanaManipulator::on_context_menu_requested);

  // 処理時間
  lbl_elapsed_time_ = new QLabel();

  // ショートカットキー (Ctrl + C)
  const auto* copy_shortcut = new QShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_C), tbl_rng_);
  connect(copy_shortcut, &QShortcut::activated, this, &ChuuHanaManipulator::copy_selection);

  // RNG Viewer
  rng_viewer_ = new RNGViewer(this);
}

void ChuuHanaManipulator::make_layouts() {
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

  auto* lo_range = new QHBoxLayout();
  auto* lbl_and = new QLabel("and");
  lo_range->addWidget(txb_range_from_);
  lo_range->addWidget(lbl_and);
  lo_range->addWidget(txb_range_to_);

  auto* lo_set_goal = new QVBoxLayout();
  auto* lbl_range = new QLabel("Value between: [-30.0, 30.0)");
  lo_set_goal->addWidget(lbl_range);
  lo_set_goal->addLayout(lo_range);
  group_set_goal_->setLayout(lo_set_goal);

  auto* lo_node = new QVBoxLayout();
  auto* lo_node_top = new QHBoxLayout();
  lo_node_top->addWidget(lbl_nodes_);
  lo_node_top->addWidget(cmb_node_range_);
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
  group_node_->setLayout(lo_node);

  auto* lo_type_auto = new QHBoxLayout();
  lo_type_auto->addWidget(rdb_type_auto_);
  lo_type_auto->addStretch(0);
  lo_type_auto->addWidget(btn_auto_left_);
  lo_type_auto->addWidget(btn_auto_right_);
  lo_type_auto->setSpacing(0);
  auto* lo_type_set_goal = new QHBoxLayout();
  lo_type_set_goal->addWidget(rdb_type_set_goal_);
  lo_type_set_goal->addWidget(group_set_goal_);
  auto* lo_type_node = new QHBoxLayout();
  lo_type_node->addWidget(rdb_type_node_);
  lo_type_node->addWidget(group_node_);
  auto* lo_top_right = new QVBoxLayout();
  lo_top_right->addLayout(lo_type_auto);
  lo_top_right->addLayout(lo_type_set_goal);
  lo_top_right->addLayout(lo_type_node);

  auto* lo_rng_information = new QHBoxLayout();
  lo_rng_information->addLayout(lo_rng);
  lo_rng_information->addStretch(1);
  lo_rng_information->addLayout(lo_top_right);

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

void ChuuHanaManipulator::update() {
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

  u32 search_range = 10000;
  search_range = txb_search_range_->text().toUInt();

  // Auto選択時
  s32 rng_type = group_rdb_rng_type_->checkedId();
  if (rng_type == TYPE_AUTO) {
    rng_type = TYPE_SET_GOAL;
    rdb_type_auto_->setText("Auto - ");
    if (DolphinComm::DolphinAccessor::getStatus() == DolphinComm::DolphinAccessor::DolphinStatus::hooked) {
      std::vector<ChuuHanaRNGType> detected_types;
      s32 at = -1;
      s32 select = -1;
      s32 priority = 0;
      for (s32 i = 0; i < 6; i++) {
        const u32 p_chuuhana = read_u32(0x81097ce0 + i * 4);
        if (read_u32(p_chuuhana + 0xf0) & 1) {
          continue;
        }
        const float x = read_float(p_chuuhana + 0x10);
        const float z = read_float(p_chuuhana + 0x18);
        const float target_x = read_float(p_chuuhana + 0x108);
        const float target_z = read_float(p_chuuhana + 0x110);
        const float dx = x - target_x;
        const float dz = z - target_z;
        const s32 timer = read_s32(p_chuuhana + 0x1a4);
        s16 collide_count = read_s16(p_chuuhana + 0x48);
        if (collide_count < 0 || 5 < collide_count)
          collide_count = 0;
        const u32 p_collide_objects = read_u32(p_chuuhana + 0x44);
        bool is_collid_move = false;
        for (u32 j = 0; j < collide_count; j++) {
          if (const u32 hit_object = read_u32(p_collide_objects + j * 4);
            read_u32(hit_object) == 0x803DAE44) {
            const s16 hit_id = read_s16(hit_object + 0x7c);
            is_collid_move = (i < hit_id);
          }
        }
        // AUTO選択時、複数の候補が上がった場合に矢印ボタンで候補を選択できる
        if (timer == 400) {
          // willFall
          ChuuHanaRNGType detected = { i, TYPE_NODE, false };
          detected_types.push_back(detected);
          at++;
          if (priority < 3) {
            select = at;
          }
          priority = 3;
        } else if (sqrt(dx * dx + dz * dz) < 100.0f) {
          // setGoal
          ChuuHanaRNGType detected = { i, TYPE_SET_GOAL, false };
          detected_types.push_back(detected);
          at++;
          if (priority < 2) {
            select = at;
          }
          priority = 2;
        } else if (is_collid_move) {
          // CollidMove
          ChuuHanaRNGType detected = { i, TYPE_NODE, true };
          detected_types.push_back(detected);
          at++;
          if (priority < 1) {
            select = at;
          }
          priority = 1;
        }
      }
      if (detected_types != detected_types_ && at >= 0) {
        detected_types_ = detected_types;
        select_search_ = select;
        select_ = select;
        if (select == 0)
          btn_auto_left_->setDisabled(true);
        else
          btn_auto_left_->setDisabled(false);
        if (select == at)
          btn_auto_right_->setDisabled(true);
        else
          btn_auto_right_->setDisabled(false);
      } else {
        select_search_ = select_;
      }
      rdb_type_auto_->setStyleSheet("");
    }
    if (!detected_types_.empty()) {
      const ChuuHanaRNGType* selected = &detected_types_[select_search_];
      rng_type = selected->rng_type;
      chb_node_collid_move_->setChecked(selected->is_collid_move);
      QString text = "Auto - ChuuHana: " + QString::number(selected->id);
      if (rng_type == TYPE_NODE) {
        if (selected->is_collid_move) {
          text += ", Type: isCollidMove";
        } else {
          text += ", Type: willFall";
        }
        if (selected->id < 3) {
          cmb_node_range_->setCurrentIndex(ZERO_TO_SEVEN);
          text += "[0 .. 7]";
        } else {
          cmb_node_range_->setCurrentIndex(ZERO_TO_SIX);
          text += "[0 .. 6]";
        }
      } else {
        text += ", Type: setGoal";
      }
      rdb_type_auto_->setText(text);
    }
  }

  if (rng_type == TYPE_SET_GOAL) {
    lbl_type_->setText("setGoal");
    const float min = txb_range_from_->text().toFloat();
    const float max = txb_range_to_->text().toFloat();
    rng_->search_rng_m30_30(min, max, search_range);
  } else if (rng_type == TYPE_NODE) {
    std::vector<s32> checked;
    lbl_type_->setText("Node");
    for (QAbstractButton* chb : group_chb_nodes_->buttons()) {
      if (chb->isChecked())
        checked.push_back(group_chb_nodes_->id(chb));
    }
    const bool is_collided = (chb_node_collid_move_->checkState() == Qt::Checked);
    s32 rng_max = 6;
    if (cmb_node_range_->currentIndex() == ZERO_TO_SEVEN) {
      rng_max = 7;
    }
    rng_->search_rng_int_array(0, rng_max, is_collided, checked, search_range);
  }

  model_tbl_rng_->update_list();

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
  } else if (index == 1) {
    group_chb_nodes_->button(7)->setDisabled(false);
  }
}

void ChuuHanaManipulator::on_chb_node_multiple_changed(s32 state) {
  if (state == Qt::Unchecked) {
    group_chb_nodes_->setExclusive(true);
  } else if (state == Qt::Checked) {
    group_chb_nodes_->setExclusive(false);
  }
}

void ChuuHanaManipulator::on_rng_type_changed(s32 id) {
  if (id == TYPE_AUTO) {
    cmb_node_range_->setDisabled(true);
    cmb_node_range_->setCurrentIndex(ZERO_TO_SEVEN);
    chb_node_collid_move_->setDisabled(true);
  } else {
    cmb_node_range_->setDisabled(false);
    chb_node_collid_move_->setDisabled(false);
  }
}

void ChuuHanaManipulator::on_context_menu_requested(const QPoint& pos) {
  QModelIndex index = tbl_rng_->indexAt(pos);
  QModelIndexList selection = tbl_rng_->selectionModel()->selectedIndexes();
  copy_selection();
  auto* menu = new QMenu(this);
  auto* copy = new QAction("Copy", this);
  connect(copy, &QAction::triggered, this, &ChuuHanaManipulator::copy_selection);
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

void ChuuHanaManipulator::copy_selection() const {
  QString str;
  auto selection = tbl_rng_->selectionModel()->selectedIndexes();
  if (selection.isEmpty())
    return;
  for (s32 i = 0; i < selection.count(); i++) {
    const auto *index = &selection[i];
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

void ChuuHanaManipulator::on_button_left_clicked() {
  select_--;
  const ChuuHanaRNGType* selected = &detected_types_[select_];
  QString text = "Auto - ChuuHana: " + QString::number(selected->id);
  if (selected->rng_type == TYPE_NODE) {
    if (selected->is_collid_move)
      text += ", Type: isCollidMove";
    else
      text += ", Type: willFall";
    if (selected->id < 3)
      text += "[0 .. 7]";
    else
      text += "[0 .. 6]";
  } else {
    text += ", Type: setGoal";
  }
  rdb_type_auto_->setText(text);

  btn_auto_right_->setDisabled(false);
  if (select_ == 0)
    btn_auto_left_->setDisabled(true);

  if (select_ == select_search_) {
    rdb_type_auto_->setStyleSheet("");
  } else {
    rdb_type_auto_->setStyleSheet("QRadioButton { color : Salmon; }");
  }
}

void ChuuHanaManipulator::on_button_right_clicked() {
  select_++;
  const ChuuHanaRNGType* selected = &detected_types_[select_];
  QString text = "Auto - ChuuHana: " + QString::number(selected->id);
  if (selected->rng_type == TYPE_NODE) {
    if (selected->is_collid_move)
      text += ", Type: isCollidMove";
    else
      text += ", Type: willFall";
    if (selected->id < 3)
      text += "[0 .. 7]";
    else
      text += "[0 .. 6]";
  } else {
    text += ", Type: setGoal";
  }
  rdb_type_auto_->setText(text);

  btn_auto_left_->setDisabled(false);
  if (select_ == detected_types_.size() - 1)
    btn_auto_right_->setDisabled(true);

  if (select_ == select_search_) {
    rdb_type_auto_->setStyleSheet("");
  } else {
    rdb_type_auto_->setStyleSheet("QRadioButton { color : Salmon; }");
  }
}
