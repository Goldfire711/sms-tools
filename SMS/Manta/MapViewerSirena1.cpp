#include "MapSirena1.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

MapViewerSirena1::MapViewerSirena1(QWidget* parent)
  : QWidget(parent) {
  map_ = new MapSirena1(nullptr);

  auto* lbl_update_timer = new QLabel();
  lbl_update_timer->setText("Update Interval(ms)");

  auto* txb_update_timer = new QLineEdit();
  txb_update_timer->setText("100");
  txb_update_timer->setFixedWidth(50);

  chb_center_on_ = new QCheckBox();
  chb_center_on_->setText("Center on Mario");

  auto* chb_show_circle = new QCheckBox();
  chb_show_circle->setText("Circle");
  chb_show_circle->setChecked(true);

  auto* chb_show_target_nodes = new QCheckBox();
  chb_show_target_nodes->setText("Target nodes");
  chb_show_target_nodes->setChecked(true);

  auto* chb_show_target = new QCheckBox();
  chb_show_target->setText("Target");
  chb_show_target->setChecked(false);

  auto* chb_show_attractor = new QCheckBox();
  chb_show_attractor->setText("Attractor");
  chb_show_attractor->setChecked(true);

  auto* chb_show_id = new QCheckBox();
  chb_show_id->setText("ID");
  chb_show_id->setChecked(true);

  auto* chb_show_dmg = new QCheckBox();
  chb_show_dmg->setText("Damage");
  chb_show_dmg->setChecked(true);

  auto* chb_show_ct = new QCheckBox();
  chb_show_ct->setText("Cool time");
  chb_show_ct->setChecked(true);

  auto* chb_show_anm = new QCheckBox();
  chb_show_anm->setText("Anm frame");
  chb_show_anm->setChecked(true);

  auto* chb_show_anm_spd = new QCheckBox();
  chb_show_anm_spd->setText("Anm speed");
  chb_show_anm_spd->setChecked(true);

  auto* chb_show_is_chasing = new QCheckBox();
  chb_show_is_chasing->setText("is targeting mario");
  chb_show_is_chasing->setChecked(true);

  auto* btn_refresh = new QPushButton();
  btn_refresh->setText("Refresh");

  auto* lo_top = new QHBoxLayout();
  lo_top->addWidget(chb_center_on_);
  lo_top->addWidget(txb_update_timer);
  lo_top->addWidget(lbl_update_timer);
  lo_top->addStretch();
  lo_top->addWidget(chb_show_circle);
  lo_top->addWidget(chb_show_target_nodes);
  lo_top->addWidget(chb_show_target);
  lo_top->addWidget(chb_show_attractor);
  lo_top->addWidget(chb_show_id);
  lo_top->addWidget(chb_show_dmg);
  lo_top->addWidget(chb_show_ct);
  lo_top->addWidget(chb_show_anm);
  lo_top->addWidget(chb_show_anm_spd);
  lo_top->addWidget(chb_show_is_chasing);
  lo_top->addWidget(btn_refresh);

  auto* lo_main = new QVBoxLayout();
  lo_main->addLayout(lo_top);
  lo_main->addWidget(map_);
  setLayout(lo_main);

  connect(chb_center_on_, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::center_on_clicked);
  connect(chb_show_circle, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_circle_clicked);
  connect(chb_show_target_nodes, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_target_nodes_clicked);
  connect(chb_show_target, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_target_clicked);
  connect(chb_show_attractor, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_attractor_clicked);
  connect(chb_show_id, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_id_clicked);
  connect(chb_show_dmg, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_dmg_clicked);
  connect(chb_show_ct, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_ct_clicked);
  connect(chb_show_anm, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_anm_clicked);
  connect(chb_show_anm_spd, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_anm_spd_clicked);
  connect(chb_show_is_chasing, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_is_chasing_clicked);
  connect(btn_refresh, &QPushButton::clicked, map_, &MapSirena1::refresh);
  connect(txb_update_timer, &QLineEdit::textChanged, this, &MapViewerSirena1::txb_update_timer_changed);
}

void MapViewerSirena1::center_on_clicked(s32 state) const {
  if (state == Qt::Unchecked)
    map_->center_on_mario_ = false;
  else
    map_->center_on_mario_ = true;
}

void MapViewerSirena1::chb_show_circle_clicked(s32 state) const {
  if (state == Qt::Checked)
    map_->show_circle_ = true;
  else
    map_->show_circle_ = false;
}

void MapViewerSirena1::chb_show_target_nodes_clicked(s32 state) const {
  if (state == Qt::Checked)
    map_->show_target_nodes_ = true;
  else
    map_->show_target_nodes_ = false;
}

void MapViewerSirena1::chb_show_target_clicked(s32 state) const{
  if (state == Qt::Checked)
    map_->show_line_target_ = true;
  else
    map_->show_line_target_ = false;
}

void MapViewerSirena1::chb_show_attractor_clicked(s32 state) const{
  if (state == Qt::Checked)
    map_->show_line_attractor_ = true;
  else
    map_->show_line_attractor_ = false;
}

void MapViewerSirena1::chb_show_id_clicked(s32 state) const{
  if (state == Qt::Checked)
    map_->show_id_ = true;
  else
    map_->show_id_ = false;
}

void MapViewerSirena1::chb_show_dmg_clicked(s32 state) const{
  if (state == Qt::Checked)
    map_->show_dmg_ = true;
  else
    map_->show_dmg_ = false;
}

void MapViewerSirena1::chb_show_ct_clicked(s32 state) const{
  if (state == Qt::Checked)
    map_->show_ct_ = true;
  else
    map_->show_ct_ = false;
}

void MapViewerSirena1::chb_show_anm_clicked(s32 state) const{
  if (state == Qt::Checked)
    map_->show_anm_ = true;
  else
    map_->show_anm_ = false;
}

void MapViewerSirena1::chb_show_anm_spd_clicked(s32 state) const{
  if (state == Qt::Checked)
    map_->show_anm_spd_ = true;
  else
    map_->show_anm_spd_ = false;
}

void MapViewerSirena1::chb_show_is_chasing_clicked(s32 state) const{
  if (state == Qt::Checked)
    map_->show_is_chasing_ = true;
  else
    map_->show_is_chasing_ = false;
}

void MapViewerSirena1::txb_update_timer_changed(QString str) const {
  bool ok = false;
  s32 interval = str.toInt(&ok);
  if (interval >= 16)
    map_->set_timer_interval(interval);
}