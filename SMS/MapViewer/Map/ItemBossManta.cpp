#include "Items.h"

ItemBossManta::ItemBossManta(const u32 p_obj, ItemManagerBase* parent)
  : ItemObjBase(p_obj, parent){
  pixmap_normal_ = QPixmap(":/sms/manta_red.png");
  pixmap_purple_ = QPixmap(":/sms/manta_purple.png");
}

void ItemBossManta::set_appearance() {
  const u32 p_ai = read_u32(p_obj_ + 0x8c);
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
  const float cos = read_float(p_obj_ + 0x170);
  const float sin = read_float(p_obj_ + 0x178);
  float rad = acos(cos);
  if (sin < 0)
    rad *= -1;
  pix_->setRotation(180 * rad / M_PI - 90);
}
