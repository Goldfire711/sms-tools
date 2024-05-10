#include "Params.h"

ParamsBase::ParamsBase(const u32 ptr) : ptr_(ptr) {
  name_ = read_string(ptr_ + 0x4, { 0x0 }, 38);
}

QVariant ParamsBase::header_data(const s32 row) const {
  return params_[row].name;
}

QVariant ParamsBase::data(const s32 row) const {
  return params_[row].calc();
}

QColor ParamsBase::data_color() const {
  return {};
}

s32 ParamsBase::row() const {
  return static_cast<s32>(params_.size());
}

QString ParamsBase::name() const {
  return name_;
}
