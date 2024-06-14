#include "MapObjectViewer.h"

MapObjectViewerItem::MapObjectViewerItem()
  : name_("Name"), class_name_("Class") {}

MapObjectViewerItem::MapObjectViewerItem(const u32 p_obj, MapObjectViewerItem* parent)
  : address_(p_obj), parent_(parent) {
  const u32 vt = read_u32(p_obj);
  std::stringstream ss;
  ss << std::hex << vt;
  if (!g_vtable_to_class.contains(ss.str()))
    return;
  const std::string class_name = g_vtable_to_class[ss.str()];
  class_name_ = QString::fromStdString(class_name);
  const QString name = read_string(p_obj + 0x4, { 0 }, 38);

  name_ = name;
}

MapObjectViewerItem::~MapObjectViewerItem() = default;

void MapObjectViewerItem::append_manager(const u32 p_manager) {
  auto* manager = append_object(p_manager);
  manager->is_manager_ = true;
  const s32 count = read_s32(p_manager + 0x14);
  manager->name_ = manager->name_ + " (" + QString::number(count) + ")";
  const u32 p_objects = read_u32(p_manager + 0x18);
  for (s32 i = 0; i < count; i++) {
    const u32 p_obj = read_u32(p_objects + 4 * i);
    auto* obj = manager->append_object(p_obj);
    if (obj->class_name_ == "TBossGesso") {
      for (s32 i = 0; i < 4; i++) {
        const u32 p_leg = read_u32(p_obj + 0x150 + i * 4, { 0x3c });
        obj->append_object(p_leg);
      }
      const u32 p_beak = read_u32(p_obj + 0x160);
      obj->append_object(p_beak);
    }
    obj->name_ = QString::number(i) + ": " + obj->name_;
  }
}

MapObjectViewerItem* MapObjectViewerItem::append_object(const u32 p_obj) {
  auto obj = std::make_unique<MapObjectViewerItem>(p_obj, this);
  children_.push_back(std::move(obj));

  return children_.back().get();
}

QVariant MapObjectViewerItem::data(const s32 column) const {
  switch (column) {
  case 0:
    return name_;
  case 1:
    return class_name_;
  case 2:
    if (parent_ == nullptr)
      return "Address";
    return "0x" + QString::number(address_, 16).toUpper();
  default:
    return {};
  }
}

s32 MapObjectViewerItem::row() const {
  if (parent_ == nullptr)
    return 0;
  const auto it = std::find_if(parent_->children_.cbegin(), parent_->children_.cend(),
    [this](const std::unique_ptr<MapObjectViewerItem> &tree_item) {
      return tree_item.get() == this;
    });

  if (it != parent_->children_.cend())
    return std::distance(parent_->children_.cbegin(), it);
  Q_ASSERT(false); // should not happen
  return -1;
}

QColor MapObjectViewerItem::data_color() const {
  if (!parent_->is_manager_)
    return {};
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return {};
  const u32 draw_info = read_u32(address_ + 0xf0);
  if (draw_info & 1)  // is inactive
    return {Qt::gray};
  if (!(draw_info & 4))  // is in camera
    return {Qt::blue};
  return {};
}
