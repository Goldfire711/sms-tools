#include "Items.h"

ItemMario::ItemMario() {
  const QPixmap pix_mario(":sms/mario.png");
  pix_ = new QGraphicsPixmapItem(pix_mario);
  addToGroup(pix_);

  circle_ = new QGraphicsEllipseItem(-100, -100, 200, 200);
  QPen pen(Qt::red);
  pen.setWidth(20);
  circle_->setPen(pen);
  addToGroup(circle_);
}

void ItemMario::update() {
  p_mario_ = read_u32(0x8040a378);
  x_ = read_float(p_mario_ + 0x10);
  y_ = read_float(p_mario_ + 0x14);
  z_ = read_float(p_mario_ + 0x18);
  spin_angle_ = read_u16(p_mario_ + 0x9a);
  atk_radius_ = read_float(p_mario_ + 0x50);

  pix_->setScale(g_obj_scale * 100 / pix_->pixmap().width());
  const QRectF bounds = pix_->boundingRect();
  pix_->setTransformOriginPoint(bounds.center());
  pix_->setTransform(QTransform().translate(-bounds.center().x(), -bounds.center().y()));
  pix_->setPos(x_, z_);
  pix_->setRotation(-spin_angle_ / 65536.0 * 360.0 - 180.0);

  circle_->setPos(x_, z_);
  //circle_->setScale(attacking_hitbox_radius_ / 200);
}
