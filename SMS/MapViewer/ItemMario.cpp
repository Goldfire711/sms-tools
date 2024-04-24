#include "Items.h"

ItemMario::ItemMario() {
  QPixmap pix_mario(":sms/mario.png");
  setPixmap(pix_mario);
  setTransformOriginPoint(pix_mario.width() / 2.0, pix_mario.height() / 2.0);
  setTransform(QTransform().translate(-pix_mario.width() / 2.0, -pix_mario.height() / 2.0));
  setScale(200.0 / pix_mario.width());
}

void ItemMario::update() {
  p_mario_ = read_u32(0x8040a378);
  x_ = read_float(p_mario_ + 0x10);
  y_ = read_float(p_mario_ + 0x14);
  z_ = read_float(p_mario_ + 0x18);
  spin_angle_ = read_u16(p_mario_ + 0x9a);

  setPos(x_, z_);
  setRotation(-spin_angle_ / 65536.0 * 360.0 - 180.0);
}