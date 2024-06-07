#include "Items.h"

ItemObjBase::ItemObjBase(const u32 p_obj, ItemManagerBase* parent) 
  : ItemBase(parent) {
  ptr_ = p_obj;
  setVisible(false);

  // Attack Radius
  QPen pen;
  pen.setWidth(20);
  pen.setColor(Qt::red);
  attack_radius_ = new QGraphicsEllipseItem;
  attack_radius_->setPen(pen);
  attack_radius_->setVisible(false);
  addToGroup(attack_radius_);

  // Receive Radius
  pen.setColor(Qt::blue);
  receive_radius_ = new QGraphicsEllipseItem;
  receive_radius_->setPen(pen);
  receive_radius_->setVisible(false);
  addToGroup(receive_radius_);

  // Attack Radius for Mario 
  pen.setWidth(20);
  pen.setColor(Qt::red);
  attack_radius_mario_ = new QGraphicsEllipseItem;
  attack_radius_mario_->setPen(pen);
  attack_radius_mario_->setVisible(false);
  addToGroup(attack_radius_mario_);

  // Receive Radius for Mario
  pen.setColor(Qt::blue);
  receive_radius_mario_ = new QGraphicsEllipseItem;
  receive_radius_mario_->setPen(pen);
  receive_radius_mario_->setVisible(false);
  addToGroup(receive_radius_mario_);

  sub_items_->push_back({ "Atk Radius", attack_radius_ });
  sub_items_->push_back({ "Dmg Radius", receive_radius_ });
  sub_items_->push_back({ "Atk Radius + Mario's Dmg Radius", attack_radius_mario_ });
  sub_items_->push_back({ "Dmg Radius + Mario's Atk Radius", receive_radius_mario_ });
}

void ItemObjBase::update(const float map_height_min, const float map_height_max) {

  x_ = read_float(ptr_ + 0x10);
  y_ = read_float(ptr_ + 0x14);
  z_ = read_float(ptr_ + 0x18);
  scale_ = read_float(ptr_ + 0x24);
  rot_y_ = read_float(ptr_ + 0x34);
  draw_info_ = read_u32(ptr_ + 0xf0);

  if (y_ <= map_height_min || map_height_max < y_) {
    setVisible(false);
    return;
  }

  set_appearance();
  set_scale();
  const QRectF bounds = pix_->boundingRect();
  pix_->setTransformOriginPoint(bounds.center());
  pix_->setTransform(QTransform().translate(-bounds.center().x(), -bounds.center().y()));
  pix_->setPos(x_, z_);
  set_rotation();
  setVisible(!(draw_info_ & 1));

  // Attack Radius
  const float atk_rad = read_float(ptr_ + 0x50);
  attack_radius_->setRect(x_ - atk_rad, z_ - atk_rad, atk_rad * 2, atk_rad * 2);

  // Receive Radius
  const float receive_rad = read_float(ptr_ + 0x58);
  receive_radius_->setRect(x_ - receive_rad, z_ - receive_rad, receive_rad * 2, receive_rad * 2);

  // Attack/Receive Radius for mario
  const u32 p_mario = read_u32(0x8040a378);
  const float mario_atk_rad = read_float(p_mario + 0x50);
  const float mario_dmg_rad = read_float(p_mario + 0x58);
  const float atk_rad_for_mario = atk_rad + mario_dmg_rad;
  const float dmg_rad_for_mario = receive_rad + mario_atk_rad;
  attack_radius_mario_->setRect(x_ - atk_rad_for_mario, z_ - atk_rad_for_mario, atk_rad_for_mario * 2, atk_rad_for_mario * 2);
  receive_radius_mario_->setRect(x_ - dmg_rad_for_mario, z_ - dmg_rad_for_mario, dmg_rad_for_mario * 2, dmg_rad_for_mario * 2);

  // Rect if is_selected
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

void ItemObjBase::set_scale() {
  pix_->setScale(scale_ * g_obj_scale);
}

void ItemObjBase::set_rotation() {
  pix_->setRotation(-rot_y_);
}

void ItemObjBase::set_appearance() {}
