#include "MapViewer.h"

#include <QComboBox>

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
  auto* chb_center_on = new QCheckBox("Center on mario");
  auto* lbl_update_timer = new QLabel("Update Interval(ms)");
  auto* txb_update_timer = new QLineEdit("33");
  txb_update_timer->setFixedWidth(50);

  auto* lbl_obj_scale = new QLabel("Obj Scale");
  auto* txb_obj_scale = new QLineEdit("2");
  txb_obj_scale->setFixedWidth(50);
  txb_obj_scale->setValidator(new QDoubleValidator());

  auto* lbl_map_layer = new QLabel("Map Layer: ");
  auto* cmb_map_layer = new QComboBox();
  cmb_map_layer->setMinimumWidth(100);
  connect(map_, &MapGeneral::map_changed, this, [cmb_map_layer](const QVector<ItemMap::map_height> &maps) {
    cmb_map_layer->clear();
    cmb_map_layer->addItem("Auto");
    if (maps.count() <= 2) {
      cmb_map_layer->setDisabled(true);
      return;
    }
    cmb_map_layer->setDisabled(false);
    for (s32 i = 0; i < maps.count() - 1; i++) {
      QString str = QString("%1: (%2, %3]").arg(i).arg(maps[i + 1].height).arg(maps[i].height);
      cmb_map_layer->addItem(str);
    }
    });
  connect(cmb_map_layer, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](const s32 index) {
    map_->set_map_layer(index - 1);
    });
  map_->init();

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

  object_parameters_ = new MapObjectParameters(this);

  // Left click event
  connect(map_, &MapGeneral::map_object_clicked, object_viewer_, &MapObjectViewer::select_item_by_address);
  connect(map_, &MapGeneral::map_object_clicked, object_parameters_, &MapObjectParameters::show_parameters);
  connect(object_viewer_, &MapObjectViewer::item_clicked, object_parameters_, &MapObjectParameters::show_parameters);
  connect(object_viewer_, &MapObjectViewer::item_clicked, map_, &MapGeneral::select_item_by_address);
  connect(object_viewer_, &MapObjectViewer::item_right_clicked, map_, &MapGeneral::show_context_menu_by_address);

  // set menu widget
  auto* lo_top = new QHBoxLayout();
  lo_top->addWidget(chb_center_on);
  lo_top->addWidget(lbl_update_timer);
  lo_top->addWidget(txb_update_timer);
  lo_top->addWidget(lbl_obj_scale);
  lo_top->addWidget(txb_obj_scale);
  lo_top->addStretch();
  lo_top->addWidget(lbl_map_layer);
  lo_top->addWidget(cmb_map_layer);
  lo_top->addWidget(btn_refresh);
  lo_top->setContentsMargins(10, 0, 10 ,0);
  auto* top = new QWidget();
  top->setLayout(lo_top);
  setMenuWidget(top);

  // set menu bar
  auto* menu_bar = new QMenuBar(nullptr);
  auto* view_menu = menu_bar->addMenu("&View");

  auto* show_object_viewer = view_menu->addAction("Show Object Viewer");
  show_object_viewer->setCheckable(true);
  show_object_viewer->setChecked(Settings::instance().IsMapObjectViewerVisible());
  connect(show_object_viewer, &QAction::toggled, &Settings::instance(), &Settings::SetMapObjectViewerVisible);
  connect(&Settings::instance(), &Settings::MapObjectViewerVisibilityChanged, show_object_viewer, &QAction::setChecked);

  auto* show_object_parameters = view_menu->addAction("Show Object Parameters");
  show_object_parameters->setCheckable(true);
  show_object_parameters->setChecked(Settings::instance().IsMapObjectParametersVisible());
  connect(show_object_parameters, &QAction::toggled, &Settings::instance(), &Settings::SetMapObjectParametersVisible);
  connect(&Settings::instance(), &Settings::MapObjectParametersVisibilityChanged, show_object_parameters, &QAction::setChecked);

  lo_top->setMenuBar(menu_bar);

  setCentralWidget(map_);
  addDockWidget(Qt::LeftDockWidgetArea, object_viewer_);
  addDockWidget(Qt::BottomDockWidgetArea, object_parameters_);

  // TODO ウィンドウ状態の復元が機能しない
  //const auto& settings = Settings::GetQSettings();
  //restoreState(settings.value(QStringLiteral("map/state")).toByteArray());
  //restoreGeometry(settings.value(QStringLiteral("map/geometry")).toByteArray());
  //Settings::instance().RefreshMapWidgetVisibility();
}

MapViewer::~MapViewer() = default;