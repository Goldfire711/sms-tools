#include "Items.h"

ItemManagerBase::ItemManagerBase(const u32 p_manager, const s32 id)
  : p_manager_(p_manager), id_(id) {
  const u32 p_objs = read_u32(p_manager_ + 0x18);
  const s32 count = read_s32(p_manager_ + 0x14);
  if (count <= 0 || 200 <= count)
    return;

  for (s32 i = 0; i < count; i++) {
    const u32 p = read_u32(p_objs + 4 * i);
    const u32 vt = read_u32(p);
    std::stringstream ss;
    ss << std::hex << vt;
    const std::string obj_class = g_vtable_to_class[ss.str()];
    // クラス名によっては専用のItemObjクラスを使う予定
    ItemObjBase* obj;
    if (obj_class == "TBossManta") {
      obj = new ItemBossManta(p, this);
    } else {
      obj = new ItemObjBase(p, this);
      if (g_class_to_png.contains(obj_class)) {
        const std::string dir = g_class_to_png[obj_class];
        const auto pix = QPixmap(QString::fromStdString(dir));
        obj->pix_->setPixmap(pix);
      }
    }
    addToGroup(obj);
    objs_.append(obj);
  }
};

ItemManagerBase::~ItemManagerBase() {
  for (auto* obj : objs_) {
    delete obj;
  }
}

void ItemManagerBase::update() {
  for (auto* obj : objs_) {
    obj->update();
  }
}
