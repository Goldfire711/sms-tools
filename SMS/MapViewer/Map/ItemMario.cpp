#include "Items.h"

ItemMario::ItemMario(QGraphicsItem* parent) : ItemBase(parent) {
  const QPixmap pix_mario(":sms/mario.png");
  pix_->setPixmap(pix_mario);

  circle_ = new QGraphicsEllipseItem(-100, -100, 200, 200);
  QPen pen(Qt::red);
  pen.setWidth(20);
  circle_->setPen(pen);
  addToGroup(circle_);
}

void ItemMario::update() {
  ptr_ = read_u32(0x8040a378);
  x_ = read_float(ptr_ + 0x10);
  y_ = read_float(ptr_ + 0x14);
  z_ = read_float(ptr_ + 0x18);
  spin_angle_ = read_u16(ptr_ + 0x9a);
  atk_radius_ = read_float(ptr_ + 0x50);

  pix_->setScale(g_obj_scale * 100 / pix_->pixmap().width());
  const QRectF bounds = pix_->boundingRect();
  pix_->setTransformOriginPoint(bounds.center());
  pix_->setTransform(QTransform().translate(-bounds.center().x(), -bounds.center().y()));
  pix_->setPos(x_, z_);
  pix_->setRotation(-spin_angle_ / 65536.0 * 360.0 - 180.0);

  circle_->setPos(x_, z_);
  //circle_->setScale(attacking_hitbox_radius_ / 200);

  if (is_selected_) {
    rect_->setRect(0, 0,
      bounds.width() * pix_->scale(),
      bounds.height() * pix_->scale());
    rect_->setPos(pix_->pos() - bounds.center() * pix_->scale());
    rect_->setVisible(true);
  } else {
    rect_->setVisible(false);
  }
}
