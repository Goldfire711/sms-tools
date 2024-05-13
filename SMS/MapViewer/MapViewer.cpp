#include "MapViewer.h"

json g_class_to_png;
qreal g_obj_scale = 2;

MapViewer::MapViewer(QWidget* parent)
  : QMainWindow(parent) {
  std::ifstream ifs("SMS/Resources/class_to_png.json");
  g_class_to_png = json::parse(ifs);

  setWindowTitle("Map Viewer");
  resize(800, 800);

  // initialize widgets
  map_ = new MapGeneral(this);
  map_->init();
  auto* chb_center_on = new QCheckBox("Center on mario");
  auto* lbl_update_timer = new QLabel("Update Interval(ms)");
  auto* txb_update_timer = new QLineEdit("33");
  txb_update_timer->setFixedWidth(50);

  auto* lbl_obj_scale = new QLabel("Obj Scale");
  auto* txb_obj_scale = new QLineEdit("2");
  txb_obj_scale->setFixedWidth(50);
  txb_obj_scale->setValidator(new QDoubleValidator());

  auto* btn_refresh = new QPushButton("Refresh");

  connect(chb_center_on, &QCheckBox::stateChanged, this, [this](const s32 state) {
    map_->set_center_on_mario(state);
    });
  connect(txb_update_timer, &QLineEdit::textChanged, this, [this](const QString &str) {
    const s32 interval = str.toInt();
    map_->set_timer_interval(interval);
    });
  connect(txb_obj_scale, &QLineEdit::textChanged, this, [=](const QString &str) {
    const qreal obj_scale = str.toDouble();
    if (1 <= obj_scale || obj_scale <= 100)
      g_obj_scale = obj_scale;
    });
  connect(btn_refresh, &QPushButton::clicked, map_, &MapGeneral::refresh);

  object_viewer_ = new MapObjectViewer(this);
  connect(btn_refresh, &QPushButton::clicked, object_viewer_, &MapObjectViewer::refresh);

  auto* bottom = new QDockWidget(this);
  object_parameters_ = new MapObjectParameters(bottom);
  bottom->setWidget(object_parameters_);

  connect(map_, &MapGeneral::map_object_clicked, object_viewer_, &MapObjectViewer::select_item_by_address);
  connect(map_, &MapGeneral::map_object_clicked, object_parameters_, &MapObjectParameters::show_parameters);
  connect(object_viewer_, &MapObjectViewer::item_clicked, object_parameters_, &MapObjectParameters::show_parameters);
  connect(object_viewer_, &MapObjectViewer::item_clicked, map_, &MapGeneral::select_item_by_address);

  // set layouts
  // TODO メニューにViewタブ追加して、ObjectViewerなどの表示非表示
  auto* lo_top = new QHBoxLayout();
  lo_top->addWidget(chb_center_on);
  lo_top->addWidget(lbl_update_timer);
  lo_top->addWidget(txb_update_timer);
  lo_top->addWidget(lbl_obj_scale);
  lo_top->addWidget(txb_obj_scale);
  lo_top->addStretch();
  lo_top->addWidget(btn_refresh);
  auto* top = new QWidget();
  top->setLayout(lo_top);
  setMenuWidget(top);

  setCentralWidget(map_);

  auto* left = new QDockWidget();
  left->setWidget(object_viewer_);
  addDockWidget(Qt::LeftDockWidgetArea, left);

  addDockWidget(Qt::BottomDockWidgetArea, bottom);
}

MapViewer::~MapViewer() = default;

