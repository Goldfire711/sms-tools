#include "MapViewer.h"

extern QTimer* g_timer_100ms;

MapViewer::MapViewer(QWidget* parent)
  : QWidget(parent) {
  ui.setupUi(this);
  map_pinna_ = new MapPinnaBeach(nullptr);
  ui.verticalLayout->addWidget(map_pinna_);
  //resize(map_pinna->size());
  connect(ui.check_center_on, &QCheckBox::stateChanged, this, &MapViewer::center_on_clicked);
}

MapViewer::~MapViewer() {
}

void MapViewer::center_on_clicked() {
  if (ui.check_center_on->checkState() == Qt::Unchecked)
    map_pinna_->center_on_mario_ = false;
  else
    map_pinna_->center_on_mario_ = true;
}
