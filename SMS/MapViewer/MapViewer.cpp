#include "MapViewer.h"

#include <QFileDialog>

extern QTimer* g_timer_100ms;

MapViewer::MapViewer(QWidget* parent)
  : QWidget(parent) {
  ui.setupUi(this);
  map_pinna_ = new MapPinnaBeach(nullptr);
  ui.verticalLayout->addWidget(map_pinna_);
  //resize(map_pinna->size());
  connect(ui.check_center_on, &QCheckBox::stateChanged, this, &MapViewer::center_on_clicked);
  connect(ui.button_capture, &QPushButton::clicked, this, &MapViewer::button_capture_clicked);
}

MapViewer::~MapViewer() {
}

void MapViewer::center_on_clicked() {
  if (ui.check_center_on->checkState() == Qt::Unchecked)
    map_pinna_->center_on_mario_ = false;
  else
    map_pinna_->center_on_mario_ = true;
}

void MapViewer::button_capture_clicked() {
  QString file_name = QFileDialog::getSaveFileName(this, "Save image", QCoreApplication::applicationDirPath(), "JPEG (*.JPEG);;PNG (*.png);;BMP Files (*.bmp)");
  if (!file_name.isNull())
  {
    map_pinna_->scene_->clearSelection();
    map_pinna_->scene_->setSceneRect(map_pinna_->rect_map_);
    QImage image(3840, 2160, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    map_pinna_->scene_->render(&painter);
    image.save(file_name);
  }
}