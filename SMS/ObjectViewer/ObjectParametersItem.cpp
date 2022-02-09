#include "ObjectParametersItem.h"
#include "../../Memory/Memory.h"

using namespace memory;

ObjectParametersItem::ObjectParametersItem() {
  type_ = Type::U32;
  string_type_ = "u32";
  address_ = 0x80000000;
  name_ = "No Name";
  notes_ = "";
}

ObjectParametersItem::ObjectParametersItem(u32 address, const nlohmann::json& json_offset, QString class_name, u32 base_offset, QString name, const bool is_pointer)
  : base_offset_(base_offset), name_(std::move(name)), class_name_(std::move(class_name)) {
  const std::string str_offset = json_offset["offset"];
  offset_ = std::stoi(str_offset, nullptr, 16);
  address_ = address + offset_ + base_offset;
  notes_ = QString::fromStdString(json_offset["notes"]);
  string_type_ = QString::fromStdString(json_offset["type"]);
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
  } else if (is_pointer) {
    type_ = Type::POINTER;
  }
}

void ObjectParametersItem::read_memory() {
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

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
    return;
  case Type::POINTER:
    string_value_ = "0x" + QString::number(read_u32(address_), 16).toUpper();
  }
}

bool ObjectParametersItem::write_memory_from_string(const QString& string) {
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return false;

  bool ok = false;
  if (type_ == Type::S8) {
    const s16 value = string.toShort(&ok);
    if (ok && -127 <= value && value <= 127)
      return write_s8(address_, (s8)value);
    return false;
  }
  if (type_ == Type::S16) {
    const s16 value = string.toShort(&ok);
    if (ok)
      return write_s16(address_, value);
    return false;
  }
  if (type_ == Type::S32) {
    const s32 value = string.toInt(&ok);
    if (ok)
      return write_s32(address_, value);
    return false;
  }
  if (type_ == Type::U8) {
    const u16 value = string.toUShort(&ok);
    if (ok && value <= 255)
      return write_u8(address_, (u8)value);
    return false;
  }
  if (type_ == Type::U16) {
    const u16 value = string.toUShort(&ok);
    if (ok)
      return write_u16(address_, value);
    return false;
  }
  if (type_ == Type::U32) {
    const u32 value = string.toUInt(&ok);
    if (ok)
      return write_u32(address_, value);
    return false;
  }
  if (type_ == Type::FLOAT) {
    const float value = string.toFloat(&ok);
    if (ok)
      return write_float(address_, value);
    return false;
  }
  if (type_ == Type::STRING) {
    const u32 address_string = read_u32(address_);
    return write_string(address_string, string);
  }
  if (type_ == Type::POINTER) {
    const u32 value_pointer = string.toUInt(&ok, 16);
    if (ok)
      return write_u32(address_, value_pointer);
    return false;
  }
  return false;
}
