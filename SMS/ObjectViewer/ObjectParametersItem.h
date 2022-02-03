#pragma once
#include <QJsonObject>
#include <QString>
#include "../../Externals/json.hpp"
#include "../../Common/CommonTypes.h"

enum class Type {
  S8,
  S16,
  S32,
  U8,
  U16,
  U32,
  FLOAT,
  STRING,
  POINTER
};

class ObjectParametersItem {
public:
  ObjectParametersItem();
  ObjectParametersItem(u32 address, const nlohmann::json& json_offset, QString class_name, u32 base_offset = 0, QString name = "", bool is_pointer = false);
  void read_memory();
  void write_memory();

  Type type_;
  u32 address_;
  u32 base_offset_;
  u32 offset_;
  QString name_ = "No Name";
  QString string_value_ = "???";
  QString string_type_;
  QString class_name_;
};
