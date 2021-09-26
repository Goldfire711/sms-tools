#include "Memory.h"
#include "../DolphinProcess/DolphinAccessor.h"

#include <string>
#include <QTextCodec>

u8 memory::read_u8(u32 address) {
  u8 value = 0;
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, &value, 1, false);
  return value;
}

u8 memory::read_u8(u32 address, std::initializer_list<u32> offsets) {
  u8 value = 0;
  for (u32 offset : offsets) {
    if (DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&address, 4, true)) {
      address += offset;
    } else {
      return 0;
    }
  }
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, &value, 1, false);
  return value;
}

u16 memory::read_u16(u32 address) {
  u16 value = 0;
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 2, true);
  return value;
}

u16 memory::read_u16(u32 address, std::initializer_list<u32> offsets) {
  u16 value = 0;
  for (u32 offset : offsets) {
    if (DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&address, 4, true)) {
      address += offset;
    } else {
      return 0;
    }
  }
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 2, true);
  return value;
}


u32 memory::read_u32(u32 address) {
  u32 value = 0;
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 4, true);
  return value;
}

u32 memory::read_u32(u32 address, std::initializer_list<u32> offsets) {
  u32 value = 0;
  for (u32 offset : offsets) {
    if (DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&address, 4, true)) {
      address += offset;
    } else {
      return 0;
    }
  }
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 4, true);
  return value;
}


s8 memory::read_s8(u32 address) {
  s8 value = 0;
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 1, false);
  return value;
}

s8 memory::read_s8(u32 address, std::initializer_list<u32> offsets) {
  s8 value = 0;
  for (u32 offset : offsets) {
    if (DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&address, 4, true)) {
      address += offset;
    } else {
      return 0;
    }
  }
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 1, false);
  return value;
}

s16 memory::read_s16(u32 address) {
  s16 value = 0;
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 2, true);
  return value;
}

s16 memory::read_s16(u32 address, std::initializer_list<u32> offsets) {
  s16 value = 0;
  for (u32 offset : offsets) {
    if (DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&address, 4, true)) {
      address += offset;
    } else {
      return 0;
    }
  }
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 2, true);
  return value;
}

s32 memory::read_s32(u32 address) {
  s32 value = 0;
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 4, true);
  return value;
}

s32 memory::read_s32(u32 address, std::initializer_list<u32> offsets) {
  s32 value = 0;
  for (u32 offset : offsets) {
    if (DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&address, 4, true)) {
      address += offset;
    } else {
      return 0;
    }
  }
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 4, true);
  return value;
}

float memory::read_float(u32 address) {
  float value;
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 4, true);
  return value;
}

float memory::read_float(u32 address, std::initializer_list<u32> offsets) {
  float value;
  for (u32 offset : offsets) {
    if (DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&address, 4, true)) {
      address += offset;
    } else {
      return 0;
    }
  }
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&value, 4, true);
  return value;
}

u8* memory::read_bytes(u32 address, size_t size) {
  auto bytes = new u8[size];
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, bytes, size, false);
  return bytes;
}

u8* memory::read_bytes(u32 address, std::initializer_list<u32> offsets, size_t size) {
  auto bytes = new u8[size];
  for (u32 offset : offsets) {
    if (DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&address, 4, true)) {
      address += offset;
    } else {
      return bytes;
    }
  }
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, bytes, size, false);
  return bytes;
}

// shift-jis
QString memory::read_string(u32 address, size_t size) {
  auto s = new u8[size + 1];
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, s, size, false);
  s[size] = '\0';
  QString qs = QTextCodec::codecForName("Shift-JIS")->toUnicode((char*)s);
  delete[] s;
  return qs;
}

// shift-jis
QString memory::read_string(u32 address, std::initializer_list<u32> offsets, size_t size) {
  auto s = new u8[size + 1];
  for (u32 offset : offsets) {
    if (DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, (u8*)&address, 4, true)) {
      address += offset;
    } else {
      return "???";
    }
  }
  if (DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, s, size, false)) {
    s[size] = '\0';
    QString qs = QTextCodec::codecForName("Shift-JIS")->toUnicode((char*)s);
    delete[] s;
    return qs;
  }
  return "???";
}

bool memory::write_float(u32 address, float value) {
  return DolphinComm::DolphinAccessor::writeToRAM(address & 0x7fffffff, (u8*)&value, 4, true);
}