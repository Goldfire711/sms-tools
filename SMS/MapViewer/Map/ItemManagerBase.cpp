#include "Items.h"

ItemManagerBase::ItemManagerBase(const u32 p_manager, QGraphicsItem* parent)
  : ItemBase(parent) {
  ptr_ = p_manager;
  const u32 p_obj_list = read_u32(ptr_ + 0x18);
  const s32 count = read_s32(ptr_ + 0x14);
  if (count <= 0 || 300 <= count)
    return;

  for (s32 i = 0; i < count; i++) {
    const u32 p = read_u32(p_obj_list + 4 * i);
    const u32 vt = read_u32(p);
    std::stringstream ss;
    ss << std::hex << vt;
    const std::string obj_class = g_vtable_to_class[ss.str()];
    ItemObjBase* obj;
    if (obj_class == "TBossManta") {
      obj = new ItemBossManta(p, this);
    } else if (obj_class == "TBossGesso") {
      obj = new ItemBossGesso(p, this);
    } else {
      obj = new ItemObjBase(p, this);
      if (g_class_to_png.contains(obj_class)) {
        const std::string dir = g_class_to_png[obj_class];
        const auto pix = QPixmap(QString::fromStdString(dir));
        obj->pix_->setPixmap(pix);
      }
    }
    addToGroup(obj);
    obj_list_.append(obj);
  }
};

ItemManagerBase::~ItemManagerBase() {
  for (auto* obj : obj_list_) {
    delete obj;
  }
}

void ItemManagerBase::update(const float map_height_min, const float map_height_max) {
  for (auto* obj : obj_list_) {
    obj->update(map_height_min, map_height_max);
  }
}
