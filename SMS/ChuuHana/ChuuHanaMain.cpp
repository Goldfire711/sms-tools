#include "ChuuHanaMain.h"
#include <QTimer>
#include <QVBoxLayout>

extern QTimer* g_timer_16ms;

ChuuHanaMain::ChuuHanaMain(QWidget* parent)
  : QTabWidget(parent) {
  resize(250, 250);
  
  auto* chuuhana_s = new ChuuHanaViewerS(nullptr);
  auto* chuuhana_m = new ChuuHanaViewerM(nullptr);
  auto* chuuhana_l = new ChuuHanaViewerL(nullptr);
  g_timer_16ms->connect(g_timer_16ms, SIGNAL(timeout()), chuuhana_s, SLOT(update()));
  g_timer_16ms->connect(g_timer_16ms, SIGNAL(timeout()), chuuhana_m, SLOT(update()));
  g_timer_16ms->connect(g_timer_16ms, SIGNAL(timeout()), chuuhana_l, SLOT(update()));

  addTab(chuuhana_s, "Mirror S");
  addTab(chuuhana_m, "Mirror M");
  addTab(chuuhana_l, "Mirror L");
}

ChuuHanaMain::~ChuuHanaMain() {
}
