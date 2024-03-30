#include "MapSirena1.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

MapViewerSirena1::MapViewerSirena1(QWidget* parent)
  : QWidget(parent) {
  map_ = new MapSirena1(nullptr);
  chb_center_on_ = new QCheckBox();
  chb_center_on_->setText("Center on Mario");

  auto* chb_show_lines = new QCheckBox();
  chb_show_lines->setText("Target lines");
  chb_show_lines->setChecked(false);

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

  auto* lo_top = new QHBoxLayout();
  lo_top->addWidget(chb_center_on_);
  lo_top->addStretch();
  lo_top->addWidget(chb_show_lines);
  lo_top->addWidget(chb_show_id);
  lo_top->addWidget(chb_show_dmg);
  lo_top->addWidget(chb_show_ct);
  lo_top->addWidget(chb_show_anm);
  lo_top->addWidget(chb_show_anm_spd);

  auto* lo_main = new QVBoxLayout();
  lo_main->addLayout(lo_top);
  lo_main->addWidget(map_);
  setLayout(lo_main);

  connect(chb_center_on_, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::center_on_clicked);
  connect(chb_show_lines, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_lines_clicked);
  connect(chb_show_id, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_id_clicked);
  connect(chb_show_dmg, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_dmg_clicked);
  connect(chb_show_ct, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_ct_clicked);
  connect(chb_show_anm, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_anm_clicked);
  connect(chb_show_anm_spd, QOverload<int>::of(&QCheckBox::stateChanged), this, &MapViewerSirena1::chb_show_anm_spd_clicked);
}

void MapViewerSirena1::center_on_clicked(s32 state) const {
  if (state == Qt::Unchecked)
    map_->center_on_mario_ = false;
  else
    map_->center_on_mario_ = true;
}

void MapViewerSirena1::chb_show_lines_clicked(s32 state) const{
  if (state == Qt::Checked)
    map_->show_lines_ = true;
  else
    map_->show_lines_ = false;
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