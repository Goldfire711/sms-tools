#include "Params.h"

ParamsObjBase::ParamsObjBase(const u32 ptr) : ParamsBase(ptr) {
  const s16 id = read_s16(ptr_ + 0x7c);
  name_ = QString::number(id) + ": " + name_;

  params_.push_back({
    "X", [this]() {
      return read_float(ptr_ + 0x10);
    }});
  params_.push_back({
    "Y", [this]() {
      return read_float(ptr_ + 0x14);
    }});
  params_.push_back({
    "Z", [this]() {
      return read_float(ptr_ + 0x18);
    }});
}

QColor ParamsObjBase::data_color() const {
  const u32 draw_info = read_u32(ptr_ + 0xf0);
  if (draw_info & 1)  // is inactive
    return { Qt::gray };
  if (!(draw_info & 4))  // is in camera
    return { Qt::blue };
  return {};
}
