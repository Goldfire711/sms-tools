#include "Params.h"

ParamsManagerBase::ParamsManagerBase(const u32 ptr) : ParamsBase(ptr) {
  params_.push_back({
    "Count", [this]() {
      return read_s32(ptr_ + 0x14);
    } });
  params_.push_back({
    "Params", [this]() {
      const u32 p_params = read_u32(ptr_ + 0x38);
      return "0x" + QString::number(p_params, 16).toUpper();
    } });
}