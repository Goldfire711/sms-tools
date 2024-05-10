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

}
