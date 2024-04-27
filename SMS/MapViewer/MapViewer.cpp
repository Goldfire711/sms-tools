#include "MapViewer.h"

json g_class_to_png;
qreal g_obj_scale = 2;

MapViewer::MapViewer(QWidget* parent)
  : QWidget(parent) {
  std::ifstream ifs("SMS/Resources/class_to_png.json");
  g_class_to_png = json::parse(ifs);

  setWindowTitle("Map Viewer");
  resize(800, 800);

  // initialize widgets
  map_ = new MapGeneral(nullptr);
  auto* chb_center_on = new QCheckBox("Center on mario");
  auto* lbl_update_timer = new QLabel("Update Interval(ms)");
  auto* txb_update_timer = new QLineEdit("33");
  txb_update_timer->setFixedWidth(50);

  auto* lbl_obj_scale = new QLabel("Obj Scale");
  auto* txb_obj_scale = new QLineEdit("2");
  txb_obj_scale->setFixedWidth(50);
  txb_obj_scale->setValidator(new QDoubleValidator());

  auto* btn_refresh = new QPushButton("Refresh");

  connect(chb_center_on, &QCheckBox::stateChanged, this, [=](const s32 state) {
    map_->set_center_on_mario(state);
    });
  connect(txb_update_timer, &QLineEdit::textChanged, this, [=](const QString str) {
    const s32 interval = str.toInt();
    map_->set_timer_interval(interval);
    });
  connect(txb_obj_scale, &QLineEdit::textChanged, this, [=](const QString str) {
    const qreal obj_scale = str.toDouble();
    if (1 <= obj_scale || obj_scale <= 100)
      g_obj_scale = obj_scale;
    });
  connect(btn_refresh, &QPushButton::clicked, map_, &MapGeneral::refresh);

  // set layout
  auto* lo_top = new QHBoxLayout();
  lo_top->addWidget(chb_center_on);
  lo_top->addWidget(lbl_update_timer);
  lo_top->addWidget(txb_update_timer);
  lo_top->addWidget(lbl_obj_scale);
  lo_top->addWidget(txb_obj_scale);
  lo_top->addStretch();
  lo_top->addWidget(btn_refresh);

  auto* lo_main = new QVBoxLayout();
  lo_main->addLayout(lo_top);
  lo_main->addWidget(map_);
  setLayout(lo_main);
}

MapViewer::~MapViewer() {
}

