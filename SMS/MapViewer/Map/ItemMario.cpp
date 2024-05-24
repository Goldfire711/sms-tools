#include "Items.h"

ItemMario::ItemMario(QGraphicsItem* parent) : ItemBase(parent) {
  const QPixmap pix_mario(":sms/mario.png");
  pix_->setPixmap(pix_mario);
  setZValue(1);

  QPen pen;
  pen.setWidth(20);

  // Attack Radius
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

  // TConductor::mGenerateRadius(Min|Max)
  pen.setColor(Qt::lightGray);
  generate_radius_max_ = new QGraphicsEllipseItem();
  generate_radius_max_->setPen(pen);
  generate_radius_min_ = new QGraphicsEllipseItem();
  generate_radius_min_->setPen(pen);
  auto generate_radius = new QGraphicsItemGroup();
  generate_radius->addToGroup(generate_radius_max_);
  generate_radius->addToGroup(generate_radius_min_);
  generate_radius->setVisible(false);
  addToGroup(generate_radius);

  sub_items_->push_back({ "Attack Radius", attack_radius_ });
  sub_items_->push_back({ "Receive Radius", receive_radius_ });
  sub_items_->push_back({ "mGenerateRadius(Min|Max)", generate_radius });
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

  // Attack Radius
  const float atk_rad = read_float(ptr_ + 0x50);
  attack_radius_->setRect(x_ - atk_rad, z_ - atk_rad, atk_rad * 2, atk_rad * 2);

  // Receive Radius
  const float receive_rad = read_float(ptr_ + 0x58);
  receive_radius_->setRect(x_ - receive_rad, z_ - receive_rad, receive_rad * 2, receive_rad * 2);

  // mGenerateRadius
  const u32 p_conductor = read_u32(0x8040a6e8);
  const float gen_rad_max = read_float(p_conductor + 0xb0);
  const float gen_rad_min = read_float(p_conductor + 0xc4);
  generate_radius_max_->setRect(x_ - gen_rad_max, z_ - gen_rad_max, gen_rad_max * 2, gen_rad_max * 2);
  generate_radius_min_->setRect(x_ - gen_rad_min, z_ - gen_rad_min, gen_rad_min * 2, gen_rad_min * 2);

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
