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