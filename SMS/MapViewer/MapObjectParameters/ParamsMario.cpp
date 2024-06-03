#include "Params.h"

ParamsMario::ParamsMario(const u32 ptr) : ParamsBase(ptr) {
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

  //params_.push_back({
  //"Anm Frame", [this]() {
  //  const u32 p_anm = read_u32(ptr_ + 0x3a8, {0xc});
  //  const float cur_anm_frame = read_float(p_anm + 0x14);
  //  const float anm_end_frame = read_s16(p_anm + 0xc);
  //  return QString::number(cur_anm_frame, 'f', 1) + "/" +
  //    QString::number(anm_end_frame, 'f', 1);
  //} });

  //params_.push_back({
  //  "Anm Speed", [this]() {
  //    const u32 p_anm = read_u32(ptr_ + 0x3a8, {0xc});
  //    const float anm_speed = read_float(p_anm + 0x10);
  //    return QString::number(anm_speed, 'f', 1) + "/f";
  //  } });

  params_.push_back({
    "Speed", [this]() {
      return read_float(ptr_ + 0xb0);
    }});

  params_.push_back({
    "Stick Angle", [this]() {
      return read_s16(ptr_ + 0x90);
    }});

  params_.push_back({
    "Facing Angle", [this]() {
      return read_s16(ptr_ + 0x96);
    }});
}