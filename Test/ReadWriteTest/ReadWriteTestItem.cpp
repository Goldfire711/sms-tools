#include "ReadWriteTestItem.h"
#include "../../Memory/Memory.h"

using namespace memory;

ReadWriteTestItem::ReadWriteTestItem() {
  type_ = TypeTest::U32;
  address_ = 0x80000000;
  name_ = "No Name";
  byte_size_ = 4;
}

ReadWriteTestItem::ReadWriteTestItem(const TypeTest type, const u32 address,
  const QString name, const size_t byte_size)
    : type_(type), address_(address), name_(name) {
  switch (type) {
  case TypeTest::S8:
    string_type_ = "s8";
    byte_size_ = 1;
    return;
  case TypeTest::U8:
    string_type_ = "u8";
    byte_size_ = 1;
    return;
  case TypeTest::S16:
    string_type_ = "s16";
    byte_size_ = 2;
    return;
  case TypeTest::U16:
    string_type_ = "u16";
    byte_size_ = 2;
    return;
  case TypeTest::S32:
    string_type_ = "s32";
    byte_size_ = 4;
    return;
  case TypeTest::U32:
    string_type_ = "u32";
    byte_size_ = 4;
    return;
  case TypeTest::FLOAT:
    string_type_ = "float";
    byte_size_ = 4;
    return;
  case TypeTest::STRING:
    string_type_ = "String";
    byte_size_ = byte_size;
    return;
  case TypeTest::BYTE_ARRAY:
    string_type_ = "byte array";
    byte_size_ = byte_size;
    return;
  }
}

ReadWriteTestItem::ReadWriteTestItem(const TypeTest type, const u32 address,
  const std::initializer_list<u32> offsets, const QString name, const size_t byte_size)
    : type_(type), address_(address), offsets_(offsets), name_(name) {
  
  switch (type) {
  case TypeTest::S8:
    string_type_ = "s8";
    byte_size_ = 1;
    return;
  case TypeTest::U8:
    string_type_ = "u8";
    byte_size_ = 1;
    return;
  case TypeTest::S16:
    string_type_ = "s16";
    byte_size_ = 2;
    return;
  case TypeTest::U16:
    string_type_ = "u16";
    byte_size_ = 2;
    return;
  case TypeTest::S32:
    string_type_ = "s32";
    byte_size_ = 4;
    return;
  case TypeTest::U32:
    string_type_ = "u32";
    byte_size_ = 4;
    return;
  case TypeTest::FLOAT:
    string_type_ = "float";
    byte_size_ = 4;
    return;
  case TypeTest::STRING:
    string_type_ = "String";
    byte_size_ = byte_size;
    return;
  case TypeTest::BYTE_ARRAY:
    string_type_ = "byte array";
    byte_size_ = byte_size;
    return;
  }
}

void ReadWriteTestItem::read_memory() {
  switch (type_) {
  case TypeTest::S8:
    string_value_ = QString::number(read_s8(address_));
    return;
  case TypeTest::S16:
    string_value_ = QString::number(read_s16(address_));
    return;
  case TypeTest::S32:
    string_value_ = QString::number(read_s32(address_));
    return;
  case TypeTest::U8:
    string_value_ = QString::number(read_u8(address_));
    return;
  case TypeTest::U16:
    string_value_ = QString::number(read_u16(address_));
    return;
  case TypeTest::U32:
    string_value_ = QString::number(read_u32(address_));
    return;
  case TypeTest::FLOAT:
    string_value_ = QString::number(read_float(address_));
    return;
  case TypeTest::BYTE_ARRAY:
    string_value_ = QByteArray((char*)read_bytes(address_, byte_size_), byte_size_).toHex(' ').toUpper();
    return;
  case TypeTest::STRING:
    string_value_ = read_string(address_, byte_size_);
    return;
  }
}

// offsets機能は未実装。こっち側でoffsetsの処理を実装する必要がある
// (↑std::initializerがうまく機能しなかった)
// offsets機能をこっち側で実装するならread_XX関連もこっちで実装したほうがいいかもしれない
// write_memory機能未実装
// アドレス等の追加・変更機能、valueの書き換え機能、16進数表示機能などを追加？
// そのうちObject Viewer(Parameters)にこれを少し改変して移植したい