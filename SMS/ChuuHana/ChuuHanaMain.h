#pragma once

#include <QMainWindow>
#include "ChuuHanaViewerS.h"
#include "ChuuHanaViewerM.h"
#include "ChuuHanaViewerL.h"

class ChuuHanaMain : public QTabWidget {
  Q_OBJECT

public:
  ChuuHanaMain(QWidget* parent = nullptr);
  ~ChuuHanaMain() override;
private:
  void tab_changed(int index);
};
