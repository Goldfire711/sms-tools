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

  // View Frustum (y=0)
  pen.setColor(Qt::lightGray);
  poly_view_frustum_ = new QGraphicsPolygonItem();
  poly_view_frustum_->setPen(pen);
  // temporary
  for (s64 i = 0; i < 6; i++) {
    poly_line_[i] = new QGraphicsLineItem();
    pen.setWidth(100);
    pen.setColor(Qt::black);
    poly_line_[i]->setPen(pen);
    addToGroup(poly_line_[i]);
  }
  for (s64 i = 0; i < 15; i++) {
    poly_combination_[i] = new QGraphicsTextItem();
    poly_combination_[i]->setScale(100);
    poly_combination_[i]->setDefaultTextColor(Qt::black);
    addToGroup(poly_combination_[i]);
  }

  addToGroup(generate_radius);
  addToGroup(poly_view_frustum_);
  addToGroup(rng_path_);

  sub_items_->push_back({ "mGenerateRadius(Min|Max)", generate_radius });
  sub_items_->push_back({ "RNG Manipulator Settings", rng_path_ });
  sub_items_->push_back({ "View Frustum (y=0)", poly_view_frustum_ });
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
  //const float rng_max_min_x = mario_x + rng_dist_max * sin(rng_angle_min * (M_PI / 180.0));
  //const float rng_max_min_z = mario_z + rng_dist_max * cos(rng_angle_min * (M_PI / 180.0));
  //const float rng_min_max_x = mario_x + rng_dist_min * sin(rng_angle_max * (M_PI / 180.0));
  //const float rng_min_max_z = mario_z + rng_dist_min * cos(rng_angle_max * (M_PI / 180.0));
  const float rng_max_max_x = mario_x + rng_dist_max * sin(rng_angle_max * (M_PI / 180.0));
  const float rng_max_max_z = mario_z + rng_dist_max * cos(rng_angle_max * (M_PI / 180.0));
  QPainterPath path;
  path.moveTo(rng_min_min_x, rng_min_min_z);
  path.arcTo(mario_x - rng_dist_min, mario_z - rng_dist_min, rng_dist_min * 2, rng_dist_min * 2, rng_angle_min - 90.0, rng_angle_max - rng_angle_min);
  path.lineTo(rng_max_max_x, rng_max_max_z);
  path.arcTo(mario_x - rng_dist_max, mario_z - rng_dist_max, rng_dist_max * 2, rng_dist_max * 2, rng_angle_max - 90.0, rng_angle_min - rng_angle_max);
  path.lineTo(rng_min_min_x, rng_min_min_z);
  rng_path_->setPath(path);

  // View Frustum (y=0)
  const u32 p_camera = read_u32(0x8040b370);
  double m[12];
  for (s64 i = 0; i < 12; i++) {
    m[i] = read_float(p_camera + 0x1ec + 4 * i);
  }
  double s[24];
  for (s64 i = 0; i < 24; i++) {
    s[i] = read_float(0x803f1c50 + i * 4);  // sViewPlane
  }
  double a[6], b[6], c[6];
  // calc coefficients of -1000 <= M(x,0,z) * (s0,s1,s2) + s3
  for (s64 i = 0; i < 6; i++) {
    a[i] = s[4 * i] * m[0] + s[1 + 4 * i] * m[4] + s[2 + 4 * i] * m[8];
    b[i] = s[4 * i] * m[2] + s[1 + 4 * i] * m[6] + s[2 + 4 * i] * m[10];
    c[i] = -s[4 * i] * m[3] - s[1 + 4 * i] * m[7] - s[2 + 4 * i] * m[11] - s[3 + 4 * i] - 1000;
  }
  // solve [[ai bi] [aj bj]][xi zi] = [ci cj]
  double x[6], z[6];
  QPolygonF poly;
  for (s64 i = 0; i < 6; i++) {
    const s64 j = (i + 1) % 6;
    const double det = a[i] * b[j] - b[i] * a[j];
    x[i] = (b[j] * c[i] - b[i] * c[j]) / det;
    z[i] = (-a[j] * c[i] + a[i] * c[j]) / det;
    poly << QPointF(x[i], z[i]);
  }
  //poly_view_frustum_->setPolygon(poly);
  s64 k = 0;
  for (s64 i = 0; i < 5; i++) {
    for (s64 j = i+1; j < 6; j++) {
      const double det = a[i] * b[j] - b[i] * a[j];
      const double x = (b[j] * c[i] - b[i] * c[j]) / det;
      const double z = (-a[j] * c[i] + a[i] * c[j]) / det;
      poly_combination_[k]->setPlainText(QString("%1,%2").arg(i).arg(j));
      poly_combination_[k]->setPos(x, z);
      k++;
    }
  }
  for (s64 i = 0; i < 6; i++) {
    constexpr double x0 = -100000.0;
    const double z0 = -a[i] * x0 / b[i] + c[i] / b[i];
    constexpr double x1 = 100000.0;
    const double z1 = -a[i] * x1 / b[i] + c[i] / b[i];
    poly_line_[i]->setLine(x0, z0, x1, z1);
  }
}
