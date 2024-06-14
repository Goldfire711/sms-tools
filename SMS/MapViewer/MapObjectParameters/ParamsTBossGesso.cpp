#include "Params.h"

ParamsTBossGesso::ParamsTBossGesso(const u32 ptr) : ParamsObjBase(ptr) {
  params_.push_back({
    "Facial pollution", [this] {
      return read_u8(ptr_ + 0x193);
    } });

  params_.push_back({
    "HP", [this] {
      return read_s8(ptr_ + 0x13c);
    }});

  params_.push_back({
    "beak_length / dmg_length", [this] {
      const float x = read_float(ptr_ + 0x10);
      const float y = read_float(ptr_ + 0x14);
      const float z = read_float(ptr_ + 0x18);
      const u32 p_beak = read_u32(ptr_ + 0x160);
      const float beak_x = read_float(p_beak + 0x10);
      const float beak_y = read_float(p_beak + 0x14);
      const float beak_z = read_float(p_beak + 0x18);
      const float dx = x - beak_x;
      const float dy = y - beak_y;
      const float dz = z - beak_z;
      float dist = dx * dx + dy * dy + dz * dz;
      if (0.0f < dist) {
        dist = sqrtf(dist);
      }
      const u32 p_param = read_u32(ptr_ + 0x70, { 0x38 });
      const float dmg_length = read_float(p_param + 0x194);

      return QString("%1/%2").arg(dist).arg(dmg_length);
    }});

}
