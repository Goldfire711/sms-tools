#include "MapGeneral.h"

MapGeneral::MapGeneral(QWidget *parent)
  : QGraphicsView(parent) {
  timer_id_ = startTimer(100);
  init();
  refresh();
}

void MapGeneral::init() {
  scale(0.1, 0.1);

  scene_ = new QGraphicsScene(this);
  setScene(scene_);

  map_ = new ItemMap();
  mario_ = new ItemMario();

  scene_->addItem(map_);
  scene_->addItem(mario_);

  QPen white_pen(QColor(0xff, 0xff, 0xff, 0x80));
  white_pen.setWidth(100);
  scene_->addRect(-5000, -5000, 5000 * 2, 5000 * 2, white_pen);
  scene_->addRect(-5425, -5425, 5425 * 2, 5425 * 2, white_pen);
  scene_->addRect(-14848, -19968, 14848 * 2, 19968 * 2, white_pen);

  setDragMode(ScrollHandDrag);
}

void MapGeneral::refresh() {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;
  // •s—v‚Èitems‚ðdelete?
  map_->set_map();
}

void MapGeneral::timerEvent(QTimerEvent* event) {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  if (map_->is_changed())
    return;
  
  mario_->update();
  map_->update(mario_->y_);

  if (center_on_mario_)
    centerOn(mario_);

  QGraphicsView::timerEvent(event);
}

void MapGeneral::wheelEvent(QWheelEvent* event) {
  qreal view_scale = 1;
  if (event->angleDelta().y() > 0) {
    view_scale = 1.1;
  } else if (event->angleDelta().y() < 0) {
    view_scale = 1 / 1.1;
  }
  scale(view_scale, view_scale);
}

void MapGeneral::set_timer_interval(s32 interval) {
  killTimer(timer_id_);
  timer_id_ = startTimer(interval);
}