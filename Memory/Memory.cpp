#include "Memory.h"
#include "../DolphinProcess/DolphinAccessor.h"

#include <string>

u8 memory::ReadValue8(const u32 address) {
  char* value;
  DolphinComm::DolphinAccessor::readFromRAM(address & 0x7fffffff, value, 7, false);
  std::string s = memory;
  delete[] memory;
  return s;
  return 0;
}
