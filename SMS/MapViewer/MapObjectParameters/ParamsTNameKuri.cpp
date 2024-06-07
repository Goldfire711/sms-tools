#include "Params.h"

ParamsTNameKuri::ParamsTNameKuri(const u32 ptr) : ParamsObjBase(ptr) {
  params_.push_back({
  "MarchSpeed", [this]() {
    return read_float(ptr_ + 0x140);
  }});

  params_.push_back({
  "BodyScale", [this]() {
    return read_float(ptr_ + 0x148);
  }});

  params_.push_back({
  "AtkR(50*scale)+mario->DmgR", [this]() {
    const u32 p_mario = read_u32(0x8040a378);
    return read_float(ptr_ + 0x50) + read_float(p_mario + 0x58);
  }});

  params_.push_back({
"DmgR(40*scale)+mario->AtkR", [this]() {
    const u32 p_mario = read_u32(0x8040a378);
    return read_float(ptr_ + 0x58) + read_float(p_mario + 0x50);
  }});

  params_.push_back({
  "AtkH(30*scale)", [this]() {
    return read_float(ptr_ + 0x54);
  }});

  params_.push_back({
  "DmgH(90*scale)", [this]() {
    return read_float(ptr_ + 0x5c);
  }});

  params_.push_back({
    "Y + AtkH", [this] {
      const float y = read_float(ptr_ + 0x14);
      const float atk_h = read_float(ptr_ + 0x54);
      return y + atk_h;
    }});

  params_.push_back({
    "Y + DmgH", [this] {
      const float y = read_float(ptr_ + 0x14);
      const float dmg_h = read_float(ptr_ + 0x5c);
      return y + dmg_h;
    }});

}
