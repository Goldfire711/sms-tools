#include "MapViewer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QCoreApplication>

MapViewer::MapViewer(QWidget* parent)
  : QWidget(parent) {
  //map_pinna_ = new MapPinnaBeach(nullptr);
  map_ = new MapGeneral(nullptr);
  chb_center_on_ = new QCheckBox();
  chb_center_on_->setText("Center on mario");
  btn_capture_ = new QPushButton();
  btn_capture_->setText("Capture");

  auto* lo_top = new QHBoxLayout();
  lo_top->addWidget(chb_center_on_);
  lo_top->addWidget(btn_capture_);

  auto* lo_main = new QVBoxLayout();
  lo_main->addLayout(lo_top);
  //lo_main->addWidget(map_pinna_);
  lo_main->addWidget(map_);
  setLayout(lo_main);

  connect(chb_center_on_, &QCheckBox::stateChanged, this, [=]() {
    if (chb_center_on_->checkState() == Qt::Unchecked)
      map_->center_on_mario_ = false;
    else
      map_->center_on_mario_ = true;
    });
  //connect(btn_capture_, &QPushButton::clicked, this, &MapViewer::button_capture_clicked);
}

MapViewer::~MapViewer() {
}

//void MapViewer::button_capture_clicked() {
//  QString file_name = QFileDialog::getSaveFileName(this, "Save image", QCoreApplication::applicationDirPath(), "JPEG (*.JPEG);;PNG (*.png);;BMP Files (*.bmp)");
//  if (!file_name.isNull())
//  {
//    map_pinna_->scene_->clearSelection();
//    map_pinna_->scene_->setSceneRect(map_pinna_->rect_map_);
//    QImage image(3840, 2160, QImage::Format_ARGB32);
//    image.fill(Qt::transparent);
//
//    QPainter painter(&image);
//    map_pinna_->scene_->render(&painter);
//    image.save(file_name);
//  }
//}