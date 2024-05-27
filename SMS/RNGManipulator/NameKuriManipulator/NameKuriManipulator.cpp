#include "NameKuriManipulator.h"

#include <QHeaderView>
#include <QShortcut>
#include <QVBoxLayout>
#include <QMenu>
#include <QApplication>
#include <QClipboard>

#include "DolphinProcess/Windows/WindowsDolphinProcess.h"
#include "Memory/Memory.h"

//NameKuriRNG::Settings NameKuriRNG::settings_;

NameKuriManipulator::NameKuriManipulator(QWidget* parent) : QWidget(parent) {
  rng_ = new NameKuriRNG();
  setWindowTitle(tr("NameKuri Manipulator"));

  // initialize widgets
  // search button
  btn_search_ = new QPushButton("Search");
  connect(btn_search_, &QPushButton::clicked, this, &NameKuriManipulator::search);

  // Radio Button to select initial seed for search
  rdb_rng_ = new RNGRadioButton(this);

  // RNG Viewer
  rng_viewer_ = new RNGViewer(this);

  // Settings
  const auto* val_float = new QRegExpValidator(QRegExp(R"(-?\d*(\.\d+)?)"));
  txb_distance_from_ = new QLineEdit(QString::number(NameKuriRNG::settings_.distance.min));
  txb_distance_to_ = new QLineEdit(QString::number(NameKuriRNG::settings_.distance.max));
  txb_angle_from_ = new QLineEdit(QString::number(NameKuriRNG::settings_.angle.min));
  txb_angle_to_ = new QLineEdit(QString::number(NameKuriRNG::settings_.angle.max));
  txb_body_scale_from_ = new QLineEdit(QString::number(NameKuriRNG::settings_.body_scale.min));
  txb_body_scale_to_ = new QLineEdit(QString::number(NameKuriRNG::settings_.body_scale.max));
  txb_march_speed_from_ = new QLineEdit(QString::number(NameKuriRNG::settings_.march_speed.min));
  txb_march_speed_to_ = new QLineEdit(QString::number(NameKuriRNG::settings_.march_speed.max));
  txb_distance_from_->setValidator(val_float);
  txb_distance_to_->setValidator(val_float);
  txb_angle_from_->setValidator(val_float);
  txb_angle_to_->setValidator(val_float);
  txb_body_scale_from_->setValidator(val_float);
  txb_body_scale_to_->setValidator(val_float);
  txb_march_speed_from_->setValidator(val_float);
  txb_march_speed_to_->setValidator(val_float);
  txb_distance_from_->setFixedWidth(60);
  txb_distance_to_->setFixedWidth(60);
  txb_angle_from_->setFixedWidth(60);
  txb_angle_to_->setFixedWidth(60);
  txb_body_scale_from_->setFixedWidth(60);
  txb_body_scale_to_->setFixedWidth(60);
  txb_march_speed_from_->setFixedWidth(60);
  txb_march_speed_to_->setFixedWidth(60);
  connect(txb_distance_from_, &QLineEdit::textChanged, this, [this](const QString& str) {
    NameKuriRNG::settings_.distance.min = str.toFloat();
    });
  connect(txb_distance_to_, &QLineEdit::textChanged, this, [this](const QString& str) {
    NameKuriRNG::settings_.distance.max = str.toFloat();
    });
  connect(txb_angle_from_, &QLineEdit::textChanged, this, [this](const QString& str) {
    NameKuriRNG::settings_.angle.min = str.toFloat();
    });
  connect(txb_angle_to_, &QLineEdit::textChanged, this, [this](const QString& str) {
    NameKuriRNG::settings_.angle.max = str.toFloat();
    });
  connect(txb_body_scale_from_, &QLineEdit::textChanged, this, [this](const QString& str) {
    NameKuriRNG::settings_.body_scale.min = str.toFloat();
    });
  connect(txb_body_scale_to_, &QLineEdit::textChanged, this, [this](const QString& str) {
    NameKuriRNG::settings_.body_scale.max = str.toFloat();
    });
  connect(txb_march_speed_from_, &QLineEdit::textChanged, this, [this](const QString& str) {
    NameKuriRNG::settings_.march_speed.min = str.toFloat();
    });
  connect(txb_march_speed_to_, &QLineEdit::textChanged, this, [this](const QString& str) {
    NameKuriRNG::settings_.march_speed.max = str.toFloat();
    });

  // probability
  lbl_probability_ = new QLabel();

  // search range
  txb_search_range_ = new QLineEdit(QString::number(rng_->search_range_));
  connect(txb_search_range_, &QLineEdit::textChanged, this, [this](const QString& str) {
    u64 search_range = str.toULongLong();
    if (search_range > 0x7fffffff) {
      search_range = 0x7fffffff;
      txb_search_range_->setText(QString::number(search_range));
    }
    rng_->search_range_ = search_range;
    });

  // RNG Table
  model_ = new NameKuriManipulatorModel(this, rng_);
  tbl_ = new QTableView(this);
  tbl_->setModel(model_);
  tbl_->setSelectionMode(QAbstractItemView::ContiguousSelection);
  tbl_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  tbl_->setContextMenuPolicy(Qt::CustomContextMenu);
  delegate_ = new NameKuriManipulatorDelegate(this);
  tbl_->setItemDelegateForColumn(1, delegate_);
  connect(tbl_, &QWidget::customContextMenuRequested, this, &NameKuriManipulator::on_context_menu_requested);

  // bottom label
  lbl_bottom_ = new QLabel();

  // shortcut key
  const auto* copy_shortcut = new QShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_C), tbl_);
  connect(copy_shortcut, &QShortcut::activated, this, &NameKuriManipulator::copy_selection);

  // copy BP
  auto* copy_bp = new QAction("Copy BP (0x802483F4)", this);
  connect(copy_bp, &QAction::triggered, this, [] {
    QApplication::clipboard()->setText("802483F4");
    });

  // Settings group
  auto* group_settings = new QGroupBox("Settings");
  group_settings->setContextMenuPolicy(Qt::ActionsContextMenu);
  group_settings->addAction(copy_bp);

  // Make layouts
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

  auto* lbl_distance = new QLabel("Distance [600, 1000): ");
  auto* lo_distance = new QHBoxLayout;
  lo_distance->addWidget(lbl_distance);
  lo_distance->addStretch();
  lo_distance->addWidget(txb_distance_from_);
  lo_distance->addWidget(new QLabel("-"));
  lo_distance->addWidget(txb_distance_to_);

  auto* lbl_angle = new QLabel("Angle [0, 360): ");
  auto* lo_angle = new QHBoxLayout;
  lo_angle->addWidget(lbl_angle);
  lo_angle->addStretch();
  lo_angle->addWidget(txb_angle_from_);
  lo_angle->addWidget(new QLabel("-"));
  lo_angle->addWidget(txb_angle_to_);

  auto* lbl_body_scale = new QLabel("Body scale [0.6, 1): ");
  auto* lo_body_scale = new QHBoxLayout;
  lo_body_scale->addWidget(lbl_body_scale);
  lo_body_scale->addStretch();
  lo_body_scale->addWidget(txb_body_scale_from_);
  lo_body_scale->addWidget(new QLabel("-"));
  lo_body_scale->addWidget(txb_body_scale_to_);

  auto* lbl_march_speed = new QLabel("March speed [1, 1.5): ");
  auto* lo_march_speed = new QHBoxLayout;
  lo_march_speed->addWidget(lbl_march_speed);
  lo_march_speed->addStretch();
  lo_march_speed->addWidget(txb_march_speed_from_);
  lo_march_speed->addWidget(new QLabel("-"));
  lo_march_speed->addWidget(txb_march_speed_to_);

  auto* lo_setting = new QVBoxLayout();
  lo_setting->addLayout(lo_distance);
  lo_setting->addLayout(lo_angle);
  lo_setting->addLayout(lo_body_scale);
  lo_setting->addLayout(lo_march_speed);

  group_settings->setLayout(lo_setting);

  auto* lo_rng_information = new QHBoxLayout();
  lo_rng_information->addLayout(lo_rng);
  lo_rng_information->addStretch(1);
  lo_rng_information->addWidget(group_settings);

  auto* lo_above_table = new QHBoxLayout();
  lo_above_table->addWidget(lbl_probability_);
  lo_above_table->addStretch(0);
  lo_above_table->addWidget(new QLabel("Search range"));
  lo_above_table->addWidget(txb_search_range_);

  auto* lo_bottom = new QHBoxLayout();
  lo_bottom->addWidget(lbl_bottom_);

  auto* lo_main = new QVBoxLayout();
  lo_main->addWidget(btn_search_);
  lo_main->addLayout(lo_rng_information);
  lo_main->addLayout(lo_above_table);
  lo_main->addWidget(tbl_);
  lo_main->addLayout(lo_bottom);

  setLayout(lo_main);

  search();
}

void NameKuriManipulator::search() const {
  const auto start = std::chrono::steady_clock::now();

  rng_->set_seed(rdb_rng_->get_seed());
  rng_->search();

  model_->update_list();

  lbl_probability_->setText(QString("1/%1").arg(rng_->probability_inv_));

  const auto end = std::chrono::steady_clock::now();
  const std::string elapsed_time = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + "ms";
  const std::string str_result_count = std::to_string(rng_->results_.size()) + " result(s) found";
  lbl_bottom_->setText(QString::fromStdString(str_result_count + " (" + elapsed_time + ")"));
}

void NameKuriManipulator::on_context_menu_requested(const QPoint& pos) {
  QModelIndex index = tbl_->indexAt(pos);
  QModelIndexList selection = tbl_->selectionModel()->selectedIndexes();
  //copy_selection();
  auto* menu = new QMenu(this);
  auto* copy = new QAction("Copy", this);
  connect(copy, &QAction::triggered, this, &NameKuriManipulator::copy_selection);
  menu->addAction(copy);
  if (selection.count() == 1) {
    auto* write_to_ram = new QAction("Write to RAM", this);
    connect(write_to_ram, &QAction::triggered, this, [this, index] {
      const QModelIndex rng_column = model_->index(index.row(), 6);
      const u32 rng = rng_column.data().toString().toUInt(nullptr, 16);
      memory::write_u32(0x80408cf0, rng);
      });
    menu->addAction(write_to_ram);
  }

  if (selection.count() != 0) {
    menu->popup(tbl_->viewport()->mapToGlobal(pos));
  }
}

void NameKuriManipulator::copy_selection() const {
  QString str;
  auto selection = tbl_->selectionModel()->selectedIndexes();
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
