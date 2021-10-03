#pragma once
#include "SMSData.h"
#include <QGraphicsView>

class MapPianta8 : public QGraphicsView {
Q_OBJECT

public:
  MapPianta8(QWidget* parent, SMSData* sms_data);
  ~MapPianta8() override;

protected:
  /*
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  */
  void timerEvent(QTimerEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

private:
  void initialize();
  QGraphicsScene* scene_;

  SMSData* sms_data_;
  QPoint last_point_;
  QPoint diff_point_;
  bool left_button_pressed_;

  QGraphicsEllipseItem* target_;
  QGraphicsRectItem* range_;
  QGraphicsEllipseItem* fluff_[32];
  QGraphicsLineItem* fluff_rail_[32];
  QGraphicsTextItem* fluff_id_[32];
  s16 former_state_[32];
  float former_x_[32];
  float former_z_[32];
  QGraphicsLineItem* fluff_warp_line_[32];

  QGraphicsTextItem* test_text_;
  s32 test_num_ = 0;

  QGraphicsEllipseItem* mario_ellipse_;
  QGraphicsPixmapItem* mario_pix_item_;

  QRectF scene_rect_;

  QGraphicsPolygonItem* blue_bird_;
  QGraphicsEllipseItem* blue_bird_next_node_;
  QGraphicsEllipseItem* blue_bird_previous_node_;

  QPixmap coin_red_pix_;
  QPixmap coin_blue_pix_;
  QGraphicsPixmapItem* coins_pix_item_[85];
  QGraphicsPixmapItem* shine_pix_item_;
};
