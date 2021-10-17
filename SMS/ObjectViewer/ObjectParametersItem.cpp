#include "ObjectParametersItem.h"
#include "../../Memory/Memory.h"

using namespace memory;

ObjectParametersItem::ObjectParametersItem() {
  type_ = Type::U32;
  string_type_ = "u32";
  address_ = 0x80000000;
  name_ = "No Name";
}

ObjectParametersItem::ObjectParametersItem(u32 address, const QJsonObject& json_offset, QString class_name)
  : class_name_(std::move(class_name)) {
  offset_ = json_offset["offset"].toString().toUInt(nullptr, 16);
  address_ = address + offset_;
  string_type_ = json_offset["type"].toString();
  name_ = json_offset["name"].toString();
  if (string_type_ == "s8") {
    type_ = Type::S8;
  } else if (string_type_ == "s16") {
    type_ = Type::S16;
  } else if (string_type_ == "s32") {
    type_ = Type::S32;
  } else if (string_type_ == "u8") {
    type_ = Type::U8;
  } else if (string_type_ == "u16") {
    type_ = Type::U16;
  } else if (string_type_ == "u32") {
    type_ = Type::U32;
  } else if (string_type_ == "float") {
    type_ = Type::FLOAT;
  } else if (string_type_ == "string") {
    type_ = Type::STRING;
  }
}

void ObjectParametersItem::read_memory() {
  switch (type_) {
  case Type::S8:
    string_value_ = QString::number(read_s8(address_));
    return;
  case Type::S16:
    string_value_ = QString::number(read_s16(address_));
    return;
  case Type::S32:
    string_value_ = QString::number(read_s32(address_));
    return;
  case Type::U8:
    string_value_ = QString::number(read_u8(address_));
    return;
  case Type::U16:
    string_value_ = QString::number(read_u16(address_));
    return;
  case Type::U32:
    string_value_ = QString::number(read_u32(address_));
    return;
  case Type::FLOAT:
    string_value_ = QString::number(read_float(address_));
    return;
  case Type::STRING:
    string_value_ = read_string(address_, { 0 }, 50);
  }
}