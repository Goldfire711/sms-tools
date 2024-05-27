#include "Items.h"

ItemBossMantaManager::ItemBossMantaManager(const u32 p_manager) : ItemManagerBase(p_manager) {
  pen_gray_ = QPen(Qt::gray);
  pen_gray_.setWidth(20);
  pen_red_ = QPen(Qt::red);
  pen_red_.setWidth(20);

  // escape radius (6000)
  circle_escape_ = new QGraphicsEllipseItem(0, 0, 12000, 12000);
  circle_escape_->setVisible(true);
  addToGroup(circle_escape_);

  sub_items_->push_back({ "Escape Radius", circle_escape_ });
}

void ItemBossMantaManager::update(float map_height_min, float map_height_max) {
  ItemManagerBase::update(map_height_min, map_height_max);

  const u32 p_mario = read_u32(0x8040a378);
  const float mario_x = read_float(p_mario + 0x10);
  const float mario_z = read_float(p_mario + 0x18);
  const s8 sEscapeFromMario = read_s8(0x8040b120);

  circle_escape_->setPos(mario_x - 6000.0, mario_z - 6000.0);
  if (sEscapeFromMario) {
    circle_escape_->setPen(pen_red_);
  } else {
    circle_escape_->setPen(pen_gray_);
  }
}
