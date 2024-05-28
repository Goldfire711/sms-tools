#include "Items.h"
#include "SMS/RNGManipulator/NameKuriManipulator/NameKuriManipulator.h"

ItemNameKuriManager::ItemNameKuriManager(const u32 p_manager) : ItemManagerBase(p_manager) {
  // TConductor::mGenerateRadius(Min|Max)
  QPen pen;
  pen.setWidth(20);
  pen.setColor(Qt::lightGray);
  generate_radius_max_ = new QGraphicsEllipseItem();
  generate_radius_max_->setPen(pen);
  generate_radius_min_ = new QGraphicsEllipseItem();
  generate_radius_min_->setPen(pen);
  auto* generate_radius = new QGraphicsItemGroup();
  generate_radius->addToGroup(generate_radius_max_);
  generate_radius->addToGroup(generate_radius_min_);
  generate_radius->setVisible(true);

  // NameKuriManipulator settings
  pen.setColor(Qt::red);
  rng_path_ = new QGraphicsPathItem();
  rng_path_->setPen(pen);

  addToGroup(generate_radius);
  addToGroup(rng_path_);

  sub_items_->push_back({ "mGenerateRadius(Min|Max)", generate_radius });
  sub_items_->push_back({ "RNG Manipulator Settings", rng_path_ });
}

void ItemNameKuriManager::update(const float map_height_min, const float map_height_max) {
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

  // NameKuriManipulator settings
  float rng_angle_min = NameKuriRNG::settings_.angle.min;
  const float rng_angle_max = NameKuriRNG::settings_.angle.max;
  rng_angle_min -= rng_angle_min > rng_angle_max ? 360.0f : 0;
  const float rng_dist_min = NameKuriRNG::settings_.distance.min;
  const float rng_dist_max = NameKuriRNG::settings_.distance.max;
  const float rng_min_min_x = mario_x + rng_dist_min * sin(rng_angle_min * (M_PI / 180.0));
  const float rng_min_min_z = mario_z + rng_dist_min * cos(rng_angle_min * (M_PI / 180.0));
  const float rng_max_min_x = mario_x + rng_dist_max * sin(rng_angle_min * (M_PI / 180.0));
  const float rng_max_min_z = mario_z + rng_dist_max * cos(rng_angle_min * (M_PI / 180.0));
  const float rng_min_max_x = mario_x + rng_dist_min * sin(rng_angle_max * (M_PI / 180.0));
  const float rng_min_max_z = mario_z + rng_dist_min * cos(rng_angle_max * (M_PI / 180.0));
  const float rng_max_max_x = mario_x + rng_dist_max * sin(rng_angle_max * (M_PI / 180.0));
  const float rng_max_max_z = mario_z + rng_dist_max * cos(rng_angle_max * (M_PI / 180.0));
  QPainterPath path;
  path.moveTo(rng_min_min_x, rng_min_min_z);
  path.arcTo(mario_x - rng_dist_min, mario_z - rng_dist_min, rng_dist_min * 2, rng_dist_min * 2, rng_angle_min - 90.0, rng_angle_max - rng_angle_min);
  path.lineTo(rng_max_max_x, rng_max_max_z);
  path.arcTo(mario_x - rng_dist_max, mario_z - rng_dist_max, rng_dist_max * 2, rng_dist_max * 2, rng_angle_max - 90.0, rng_angle_min - rng_angle_max);
  path.lineTo(rng_min_min_x, rng_min_min_z);
  rng_path_->setPath(path);
}
