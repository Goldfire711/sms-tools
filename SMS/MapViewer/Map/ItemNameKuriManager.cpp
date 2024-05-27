#include "Items.h"

ItemNameKuriManager::ItemNameKuriManager(const u32 p_manager) : ItemManagerBase(p_manager) {
  // TConductor::mGenerateRadius(Min|Max)
  QPen pen;
  pen.setWidth(20);
  pen.setColor(Qt::lightGray);
  generate_radius_max_ = new QGraphicsEllipseItem();
  generate_radius_max_->setPen(pen);
  generate_radius_min_ = new QGraphicsEllipseItem();
  generate_radius_min_->setPen(pen);
  auto generate_radius = new QGraphicsItemGroup();
  generate_radius->addToGroup(generate_radius_max_);
  generate_radius->addToGroup(generate_radius_min_);
  generate_radius->setVisible(true);
  addToGroup(generate_radius);

  sub_items_->push_back({ "mGenerateRadius(Min|Max)", generate_radius });
}

void ItemNameKuriManager::update(float map_height_min, float map_height_max) {
  ItemManagerBase::update(map_height_min, map_height_max);

  const u32 p_mario = read_u32(0x8040a378);
  const float mario_x = read_float(p_mario + 0x10);
  const float mario_z = read_float(p_mario + 0x18);

  // mGenerateRadius
  const u32 p_conductor = read_u32(0x8040a6e8);
  const float gen_rad_max = read_float(p_conductor + 0xb0);
  const float gen_rad_min = read_float(p_conductor + 0xc4);
  generate_radius_max_->setRect(mario_x - gen_rad_max, mario_z - gen_rad_max, gen_rad_max * 2, gen_rad_max * 2);
  generate_radius_min_->setRect(mario_x - gen_rad_min, mario_z - gen_rad_min, gen_rad_min * 2, gen_rad_min * 2);
}
