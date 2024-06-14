#include "Items.h"

ItemBossGesso::ItemBossGesso(const u32 p_obj, ItemManagerBase* parent)
    : ItemObjBase(p_obj, parent) {
  pix_->setPixmap(QPixmap("SMS/Resources/Images/bgeso_x1.png"));

  QPen pen(Qt::lightGray);
  pen.setWidth(20);

  // manager->params->mSLBeakLengthDamage
  beak_length_damage_ = new QGraphicsEllipseItem;
  beak_length_damage_->setPen(pen);
  addToGroup(beak_length_damage_);

  // tentacles
  pen.setColor(Qt::blue);
  for (s32 i = 0; i < 4; i++) {
    tentacle_[i] = new QGraphicsEllipseItem;
    tentacle_[i]->setPen(pen);
    addToGroup(tentacle_[i]);
  }
  // beak
  beak_ = new QGraphicsEllipseItem;
  beak_->setPen(pen);
  addToGroup(beak_);

  sub_items_->push_back({ "mSLBeakLengthDamage", beak_length_damage_ });
}

void ItemBossGesso::update(const float map_height_min, const float map_height_max) {
  ItemObjBase::update(map_height_min, map_height_max);

  // manager->params->mSLBeakLengthDamage
  const u32 p_manager = read_u32(ptr_ + 0x70);
  const u32 p_params = read_u32(p_manager + 0x38);
  const float beak_dmg_rad = read_float(p_params + 0x194);
  beak_length_damage_->setRect(x_ - beak_dmg_rad, z_ - beak_dmg_rad, beak_dmg_rad * 2, beak_dmg_rad * 2);

  // tentacles
  const u32 p_mario = read_u32(0x8040a378);
  const float mario_atk_r = read_float(p_mario + 0x50);
  for (s32 i = 0; i < 4; i++) {
    const u32 p = read_u32(ptr_ + 0x150 + i * 4);
    const u32 p_hit = read_u32(p + 0x3c);
    const float x = read_float(p_hit + 0x10);
    const float z = read_float(p_hit + 0x18);
    const float dmg_r = read_float(p_hit + 0x58);
    const float r = dmg_r + mario_atk_r;
    tentacle_[i]->setRect(x - r, z - r, r * 2, r * 2);
  }

  // beak
  const u32 p_beak = read_u32(ptr_ + 0x160);
  const float beak_x = read_float(p_beak + 0x10);
  const float beak_z = read_float(p_beak + 0x18);
  const float beak_dmg_r = read_float(p_beak + 0x58);
  const float beak_r = beak_dmg_r + mario_atk_r;
  beak_->setRect(beak_x - beak_r, beak_z - beak_r, beak_r * 2, beak_r * 2);
}
