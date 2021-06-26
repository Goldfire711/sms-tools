#pragma once

#include <QGraphicsView>

class MapPinnaBeach : public QGraphicsView {
Q_OBJECT

public:
  MapPinnaBeach(QWidget* parent);
  ~MapPinnaBeach() override;

  bool center_on_mario_ = false;

protected:
  void timerEvent(QTimerEvent* event) override;

private:
  void initialize();
  QGraphicsPixmapItem* set_map_image(const QPixmap& pixmap, double pix_x1, double pix_y1, double pix_x2, double game_x1, double game_y1, double game_x2);

  QGraphicsScene* scene_;
  QGraphicsPixmapItem* mario_pix_item_;
  QGraphicsPixmapItem* water_hit_obj_items_[200];
  QGraphicsPixmapItem* item_manager_items_[100];
  QPixmap coin_pix_;
  QPixmap coin_blue_pix_;
  QPixmap fruit_banana_pix_;
  QPixmap fruit_coconut_pix_;
  QPixmap fruit_durian_pix_;
  QPixmap fruit_papaya_pix_;
  QPixmap fruit_pine_pix_;

};
