#include "Items.h"

ItemBossManta::ItemBossManta(const u32 p_obj, ItemManagerBase* parent)
  : ItemObjBase(p_obj, parent){
  pixmap_normal_ = QPixmap(":/sms/manta_red.png");
  pixmap_purple_ = QPixmap(":/sms/manta_purple.png");

  // Target
  QPen pen(Qt::blue);
  pen.setWidth(20);
  target_line_ = new QGraphicsLineItem;
  target_line_->setPen(pen);
  target_line_->setVisible(false);
  addToGroup(target_line_);

  // Attractor
  pen.setColor(Qt::red);
  attractor_line_ = new QGraphicsLineItem;
  attractor_line_->setPen(pen);
  addToGroup(attractor_line_);

  sub_items_->push_back({ "Target", target_line_ });
  sub_items_->push_back({ "Attractor", attractor_line_ });
}

void ItemBossManta::update(const float map_height_min, const float map_height_max) {
  ItemObjBase::update(map_height_min, map_height_max);

  // Target
  const float target_x = read_float(ptr_ + 0x158);
  const float target_z = read_float(ptr_ + 0x160);
  target_line_->setLine(x_, z_, target_x, target_z);

  // Attractor
  const float attractor_x = read_float(ptr_ + 0x164);
  const float attractor_z = read_float(ptr_ + 0x16c);
  const float attractor_sqrt_inv = 1 / sqrt(attractor_x * attractor_x + attractor_z * attractor_z);
  attractor_line_->setLine(x_, z_,
    x_ + 300 * attractor_x * attractor_sqrt_inv, z_ + 300 * attractor_z * attractor_sqrt_inv);
}

void ItemBossManta::set_appearance() {
  const u32 p_ai = read_u32(ptr_ + 0x8c);
  const u32 cur_nerve = read_u32(p_ai + 0x14);
  if (cur_nerve == 0x8040B0F4) {
    pix_->setPixmap(pixmap_purple_);
  } else {
    pix_->setPixmap(pixmap_normal_);
  }
}

void ItemBossManta::set_scale() {
  pix_->setScale(scale_);
}

void ItemBossManta::set_rotation() {
  const float cos = read_float(ptr_ + 0x170);
  const float sin = read_float(ptr_ + 0x178);
  float rad = acos(cos);
  if (sin < 0)
    rad *= -1;
  pix_->setRotation(180 * rad / M_PI - 90);
}
