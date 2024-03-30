#pragma once

#include "../../Common/CommonTypes.h"
#include <QGraphicsView>
#include <QWidget>
#include <QCheckBox>

class MapSirena1 : public QGraphicsView {
  Q_OBJECT

public:
  MapSirena1(QWidget *parent);
  ~MapSirena1();

  bool center_on_mario_ = false;
  bool show_lines_ = false;
  bool show_id_ = true;
  bool show_dmg_ = true;
  bool show_ct_ = true;
  bool show_anm_ = true;
  bool show_anm_spd_ = true;

protected:
  void timerEvent(QTimerEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

private:
  void initialize();

  QGraphicsPixmapItem* mario_;
  QGraphicsPixmapItem* manta_[128];
  QGraphicsLineItem* manta_line1_[128];
  QGraphicsLineItem* manta_line2_[128];
  QGraphicsTextItem* manta_id_[128];
  QPixmap pix_manta_;
  QPixmap pix_manta_purple_;
};

class MapViewerSirena1 : public QWidget {
  Q_OBJECT

public:
  MapViewerSirena1(QWidget* parent = Q_NULLPTR);

private:
  MapSirena1* map_;
  QCheckBox* chb_center_on_;

  void center_on_clicked(s32 state) const;
  void chb_show_lines_clicked(s32 state) const;
  void chb_show_id_clicked(s32 state) const;
  void chb_show_dmg_clicked(s32 state) const;
  void chb_show_ct_clicked(s32 state) const;
  void chb_show_anm_clicked(s32 state) const;
  void chb_show_anm_spd_clicked(s32 state) const;
};