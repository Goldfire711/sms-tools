#ifdef _WIN32

#pragma once

#include <windows.h>

#include "../IDolphinProcess.h"

namespace DolphinComm {
class WindowsDolphinProcess : public IDolphinProcess {
public:
  WindowsDolphinProcess() {
  }

  bool findPID() override;
  bool obtainEmuRAMInformations() override;
  bool readFromRAM(u32 offset, u8* buffer, size_t size,
                   bool withBSwap) override;
  bool writeToRAM(u32 offset, const u8* buffer, size_t size,
                  bool withBSwap) override;

private:
  HANDLE m_hDolphin;
};
} // namespace DolphinComm
#endif
