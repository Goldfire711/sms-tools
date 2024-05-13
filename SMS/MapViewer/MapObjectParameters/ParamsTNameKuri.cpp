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
  "AttackRadius (50*scale)", [this]() {
    return read_float(ptr_ + 0x50);
  }});

  params_.push_back({
  "AttackHeight (30*scale)", [this]() {
    return read_float(ptr_ + 0x54);
  }});

  params_.push_back({
  "DamageRadius (40*scale)", [this]() {
    return read_float(ptr_ + 0x58);
  }});

  params_.push_back({
  "DamageHeight (90*scale)", [this]() {
    return read_float(ptr_ + 0x5c);
  }});

}
