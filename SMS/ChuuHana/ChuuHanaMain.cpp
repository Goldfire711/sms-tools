#include "ChuuHanaMain.h"
#include <QTimer>
#include <QVBoxLayout>
#include <QDebug>

extern QTimer* g_timer_16ms;

ChuuHanaMain::ChuuHanaMain(QWidget* parent)
  : QTabWidget(parent) {
  resize(750, 250);
  connect(this, &QTabWidget::currentChanged, this, &ChuuHanaMain::tab_changed);

  auto* chuuhana_s2 = new ChuuHanaViewerS(nullptr);
  auto* chuuhana_m2 = new ChuuHanaViewerM(nullptr);
  auto* chuuhana_l2 = new ChuuHanaViewerL(nullptr);
  g_timer_16ms->connect(g_timer_16ms, SIGNAL(timeout()), chuuhana_s2, SLOT(update()));
  g_timer_16ms->connect(g_timer_16ms, SIGNAL(timeout()), chuuhana_m2, SLOT(update()));
  g_timer_16ms->connect(g_timer_16ms, SIGNAL(timeout()), chuuhana_l2, SLOT(update()));
  QWidget* widget_all_h = new QWidget();
  QHBoxLayout* all_layout_h = new QHBoxLayout();
  all_layout_h->addWidget(chuuhana_s2);
  all_layout_h->addWidget(chuuhana_m2);
  all_layout_h->addWidget(chuuhana_l2);
  widget_all_h->setLayout(all_layout_h);
  addTab(widget_all_h, "All(H)");

  auto* chuuhana_s3 = new ChuuHanaViewerS(nullptr);
  auto* chuuhana_m3 = new ChuuHanaViewerM(nullptr);
  auto* chuuhana_l3 = new ChuuHanaViewerL(nullptr);
  g_timer_16ms->connect(g_timer_16ms, SIGNAL(timeout()), chuuhana_s3, SLOT(update()));
  g_timer_16ms->connect(g_timer_16ms, SIGNAL(timeout()), chuuhana_m3, SLOT(update()));
  g_timer_16ms->connect(g_timer_16ms, SIGNAL(timeout()), chuuhana_l3, SLOT(update()));
  QWidget* widget_all_v = new QWidget();
  QVBoxLayout* main_layout_v = new QVBoxLayout();
  main_layout_v->addWidget(chuuhana_s3);
  main_layout_v->addWidget(chuuhana_m3);
  main_layout_v->addWidget(chuuhana_l3);
  widget_all_v->setLayout(main_layout_v);
  addTab(widget_all_v, "All(V)");
  
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

void ChuuHanaMain::tab_changed(int index) {
  int side = qMin(width(), height());
  if (index == 0) {
    resize(side * 3, side);
  }
  else if (index == 1) {
    resize(side, side * 3);
  }
  else {
    resize(side, side);
  }
}
