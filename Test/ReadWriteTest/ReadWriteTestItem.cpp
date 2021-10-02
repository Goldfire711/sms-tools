#include "ReadWriteTestItem.h"
#include "../../Memory/Memory.h"

using namespace memory;

ReadWriteTestItem::ReadWriteTestItem() {
  type_ = Type::U32;
  address_ = 0x80000000;
  name_ = "No Name";
  byte_size_ = 4;
}

ReadWriteTestItem::ReadWriteTestItem(const Type type, const u32 address,
  const QString name, const size_t byte_size)
    : type_(type), address_(address), name_(name) {
  switch (type) {
  case Type::S8:
    string_type_ = "s8";
    byte_size_ = 1;
    return;
  case Type::U8:
    string_type_ = "u8";
    byte_size_ = 1;
    return;
  case Type::S16:
    string_type_ = "s16";
    byte_size_ = 2;
    return;
  case Type::U16:
    string_type_ = "u16";
    byte_size_ = 2;
    return;
  case Type::S32:
    string_type_ = "s32";
    byte_size_ = 4;
    return;
  case Type::U32:
    string_type_ = "u32";
    byte_size_ = 4;
    return;
  case Type::FLOAT:
    string_type_ = "float";
    byte_size_ = 4;
    return;
  case Type::STRING:
    string_type_ = "String";
    byte_size_ = byte_size;
    return;
  case Type::BYTE_ARRAY:
    string_type_ = "byte array";
    byte_size_ = byte_size;
    return;
  }
}

ReadWriteTestItem::ReadWriteTestItem(const Type type, const u32 address,
  const std::initializer_list<u32> offsets, const QString name, const size_t byte_size)
    : type_(type), address_(address), offsets_(offsets), name_(name) {
  
  switch (type) {
  case Type::S8:
    string_type_ = "s8";
    byte_size_ = 1;
    return;
  case Type::U8:
    string_type_ = "u8";
    byte_size_ = 1;
    return;
  case Type::S16:
    string_type_ = "s16";
    byte_size_ = 2;
    return;
  case Type::U16:
    string_type_ = "u16";
    byte_size_ = 2;
    return;
  case Type::S32:
    string_type_ = "s32";
    byte_size_ = 4;
    return;
  case Type::U32:
    string_type_ = "u32";
    byte_size_ = 4;
    return;
  case Type::FLOAT:
    string_type_ = "float";
    byte_size_ = 4;
    return;
  case Type::STRING:
    string_type_ = "String";
    byte_size_ = byte_size;
    return;
  case Type::BYTE_ARRAY:
    string_type_ = "byte array";
    byte_size_ = byte_size;
    return;
  }
}

void ReadWriteTestItem::read_memory() {
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
  case Type::BYTE_ARRAY:
    string_value_ = QByteArray((char*)read_bytes(address_, byte_size_), byte_size_).toHex(' ').toUpper();
    return;
  case Type::STRING:
    string_value_ = read_string(address_, byte_size_);
    return;
  }
}

// offsets�@�\�͖������B����������offsets�̏�������������K�v������
// (��std::initializer�����܂��@�\���Ȃ�����)
// offsets�@�\�����������Ŏ�������Ȃ�read_XX�֘A���������Ŏ��������ق���������������Ȃ�
// write_memory�@�\������
// �A�h���X���̒ǉ��E�ύX�@�\�Avalue�̏��������@�\�A16�i���\���@�\�Ȃǂ�ǉ��H
// ���̂���Object Viewer(Parameters)�ɂ�����������ς��ĈڐA������