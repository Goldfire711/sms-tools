#include "MapViewer.h"

json g_class_to_png;

MapViewer::MapViewer(QWidget* parent)
  : QWidget(parent) {
  std::ifstream ifs("SMS/Resources/class_to_png.json");
  g_class_to_png = json::parse(ifs);

  // initialize widgets
  map_ = new MapGeneral(nullptr);
  auto* chb_center_on = new QCheckBox("Center on mario");
  auto* btn_refresh = new QPushButton("Refresh");
  auto* lbl_update_timer = new QLabel("Update Interval(ms)");
  auto* txb_update_timer = new QLineEdit("33");
  txb_update_timer->setFixedWidth(50);

  connect(chb_center_on, &QCheckBox::stateChanged, this, [=](const s32 state) {
    map_->set_center_on_mario(state);
    });
  connect(btn_refresh, &QPushButton::clicked, map_, &MapGeneral::refresh);
  connect(txb_update_timer, &QLineEdit::textChanged, this, [=](const QString str) {
    const s32 interval = str.toInt();
    map_->set_timer_interval(interval);
    });

  // set layout
  auto* lo_top = new QHBoxLayout();
  lo_top->addWidget(chb_center_on);
  lo_top->addWidget(txb_update_timer);
  lo_top->addWidget(lbl_update_timer);
  lo_top->addStretch();
  lo_top->addWidget(btn_refresh);

  auto* lo_main = new QVBoxLayout();
  lo_main->addLayout(lo_top);
  lo_main->addWidget(map_);
  setLayout(lo_main);
}

MapViewer::~MapViewer() {
}

