#pragma once
#include <initializer_list>
#include <QString>

#include "../Common/CommonTypes.h"
#include "../DolphinProcess/DolphinAccessor.h"

namespace memory {
u8 read_u8(u32 address);
u8 read_u8(u32 address, std::initializer_list<u32> offsets);
u16 read_u16(u32 address);
u16 read_u16(u32 address, std::initializer_list<u32> offsets);
u32 read_u32(u32 address);
u32 read_u32(u32 address, std::initializer_list<u32> offsets);
s8 read_s8(u32 address);
s8 read_s8(u32 address, std::initializer_list<u32> offsets);
s16 read_s16(u32 address);
s16 read_s16(u32 address, std::initializer_list<u32> offsets);
s32 read_s32(u32 address);
s32 read_s32(u32 address, std::initializer_list<u32> offsets);
float read_float(u32 address);
float read_float(u32 address, std::initializer_list<u32> offsets);
u8* read_bytes(u32 address, size_t size);
u8* read_bytes(u32 address, std::initializer_list<u32> offsets, size_t size);
QString read_string(u32 address, size_t size);
QString read_string(u32 address, std::initializer_list<u32> offsets, size_t size);
bool write_u8(u32 address, u8 value);
bool write_u16(u32 address, u16 value);
bool write_u32(u32 address, u32 value);
bool write_s8(u32 address, s8 value);
bool write_s16(u32 address, s16 value);
bool write_s32(u32 address, s32 value);
bool write_float(u32 address, float value);
bool write_string(u32 address, const QString& string);

} // namespace memory
