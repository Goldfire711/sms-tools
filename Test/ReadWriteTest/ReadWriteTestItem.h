#pragma once
#include <QString>
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
  BYTE_ARRAY
};

class ReadWriteTestItem {
public:
  ReadWriteTestItem();
  ReadWriteTestItem(Type type, u32 address, QString name = "No Name", size_t byte_size = 16);
  ReadWriteTestItem(Type type, u32 address, std::initializer_list<u32> offsets,
    QString name = "No Name", size_t byte_size = 16);
  void read_memory();
  void write_memory();
  QString get_type_string();

  Type type_;
  u32 address_;
  std::initializer_list<u32> offsets_;
  QString name_ = "No Name";
  size_t byte_size_ = 16;
  QString string_value_ = "???";
  QString string_type_;
};
