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
};
