#include "Params.h"

ParamsTBossManta::ParamsTBossManta(const u32 ptr) : ParamsObjBase(ptr) {
  params_.push_back({
    "Form", [this]() {
      return read_s32(ptr_ + 0x18c);
    }});

  params_.push_back({
    "Damage/HP", [this]() {
      const s32 damage = read_s32(ptr_ + 0x19c);
      const s32 form = read_s32(ptr_ + 0x18c);
      constexpr s32 hps[] = { 16, 8, 4, 2, 1, 1 };
      s32 hp = -1;
      if (0 <= form && form < 6)
        hp = hps[form];
      return QString::number(damage) + "/" + QString::number(hp);
    }});

  params_.push_back({
    "CT", [this]() {
      return read_s32(ptr_ + 0x1a0);
    }});

  params_.push_back({
    "isTargetingMario", [this]() {
      const s32 is_targeting_mario = read_s32(ptr_ + 0x1a4);
      const u32 p_ai = read_u32(ptr_ + 0x8c);
      const s32 timer = read_s32(p_ai + 0x20);
      const s32 escape_time = read_s32(ptr_ + 0x188);
      const s32 timer_mod = escape_time ? timer % escape_time : 0;
      return QString::number(is_targeting_mario) + ": " +
        QString::number(timer_mod) + "/" +
        QString::number(escape_time);
    }});

  params_.push_back({
    "sEscapeFromMario", [] {
      return read_s8(0x8040b120);
    }});
}
