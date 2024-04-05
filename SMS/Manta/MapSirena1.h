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
  void refresh();
  void set_timer_interval(s32 interval);

  bool center_on_mario_ = false;
  bool show_circle_ = true;
  bool show_target_nodes_ = true;
  bool show_line_target_ = false;
  bool show_line_attractor_ = true;
  bool show_id_ = true;
  bool show_dmg_ = true;
  bool show_ct_ = true;
  bool show_anm_ = true;
  bool show_anm_spd_ = true;
  bool show_is_chasing_ = true;
  u32 p_manta_manager_ = 0;

protected:
  void timerEvent(QTimerEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

private:
  void initialize();

  QGraphicsPixmapItem* mario_;
  QGraphicsPixmapItem* manta_[128];
  QGraphicsLineItem* manta_target_line_[128];
  QGraphicsLineItem* manta_attractor_line_[128];
  QGraphicsTextItem* manta_info_[128];
  QPixmap pix_manta_;
  QPixmap pix_manta_purple_;
  QVector<QPoint> manta_nodes_;
  QGraphicsPolygonItem* manta_nodes_poly_;
  QGraphicsTextItem* manta_nodes_poly_id_[13];
  QGraphicsPolygonItem* manta_targets1_;
  QGraphicsTextItem* manta_target_id1_[13];
  QGraphicsPolygonItem* manta_targets2_;
  QGraphicsTextItem* manta_target_id2_[13];
  QGraphicsEllipseItem* circle6000_;
  QPen pen_gray_;
  QPen pen_red_;

  s32 timer_id_ = -1;
};

class MapViewerSirena1 : public QWidget {
  Q_OBJECT

public:
  MapViewerSirena1(QWidget* parent = Q_NULLPTR);

private:
  MapSirena1* map_;
  QCheckBox* chb_center_on_;

  void center_on_clicked(s32 state) const;
  void chb_show_circle_clicked(s32 state) const;
  void chb_show_target_nodes_clicked(s32 state) const;
  void chb_show_target_clicked(s32 state) const;
  void chb_show_attractor_clicked(s32 state) const;
  void chb_show_id_clicked(s32 state) const;
  void chb_show_dmg_clicked(s32 state) const;
  void chb_show_ct_clicked(s32 state) const;
  void chb_show_anm_clicked(s32 state) const;
  void chb_show_anm_spd_clicked(s32 state) const;
  void chb_show_is_chasing_clicked(s32 state) const;
  void txb_update_timer_changed(QString str) const;
};