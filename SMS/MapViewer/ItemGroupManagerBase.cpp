#include "Items.h"

ItemGroupManagerBase::ItemGroupManagerBase(const u32 p_manager)
  : p_manager_(p_manager) {
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
    auto* obj = new ItemObjBase(p, obj_class);
    addToGroup(obj);
    objs_.append(obj);
  }
};

ItemGroupManagerBase::~ItemGroupManagerBase() {
  for (auto* obj : objs_) {
    delete obj;
  }
}

void ItemGroupManagerBase::update() {
  for (auto* obj : objs_) {
    obj->update();
  }
}