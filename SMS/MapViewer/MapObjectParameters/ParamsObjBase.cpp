#include "Params.h"
#include "SMS/MapViewer/MapObjectViewer/MapObjectViewer.h"

ParamsObjBase::ParamsObjBase(const u32 ptr) : ParamsBase(ptr) {
  const s16 id = read_s16(ptr_ + 0x7c);
  name_ = QString::number(id) + ": " + name_;

  //params_.push_back({
  //  "id", [this]() {
  //    return read_s16(ptr_ + 0x7c);
  //  }});

  //params_.push_back({
  //  "is inactive", [this]() {
  //    return read_u32(ptr_ + 0xf0) & 1;
  //  }});

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

  params_.push_back({
"Nerve", [this]() {
  const u32 p_vt_nerve = read_u32(ptr_ + 0x8c, {0x14});
  const u32 vt_nerve = read_u32(p_vt_nerve);
  std::stringstream ss;
  ss << std::hex << vt_nerve;
  if (g_vtable_to_class.contains(ss.str())) {
    return QString::fromStdString(g_vtable_to_class[ss.str()]);
  }
  return QString::number(p_vt_nerve, 16).toUpper();
    }});


  params_.push_back({
  "Anm Frame", [this]() {
    const u32 p_anm = read_u32(ptr_ + 0x74, {0xc});
    const float cur_anm_frame = read_float(p_anm + 0x14);
    const float anm_end_frame = read_s16(p_anm + 0xc);
    return QString::number(cur_anm_frame, 'f', 1) + "/" +
      QString::number(anm_end_frame, 'f', 1);
    }});

  params_.push_back({
    "Anm Speed", [this]() {
      const u32 p_anm = read_u32(ptr_ + 0x74, {0xc});
      const float anm_speed = read_float(p_anm + 0x10);
      return QString::number(anm_speed, 'f', 1) + "/f";
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
