#ifdef __linux__

#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include "../IDolphinProcess.h"

namespace DolphinComm
{
class LinuxDolphinProcess : public IDolphinProcess
{
public:
  LinuxDolphinProcess()
  {
  }
  bool findPID() override;
  bool obtainEmuRAMInformations() override;
  bool readFromRAM(const u32 offset, u8* buffer, size_t size, const bool withBSwap) override;
  bool writeToRAM(const u32 offset, const u8* buffer, const size_t size,
                  const bool withBSwap) override;
};
} // namespace DolphinComm
#endif
