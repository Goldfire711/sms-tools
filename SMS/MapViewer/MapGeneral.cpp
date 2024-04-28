#include "MapGeneral.h"

MapGeneral::MapGeneral(QWidget *parent)
  : QGraphicsView(parent) {
  timer_id_ = startTimer(33);
}

void MapGeneral::init() {
  scale(0.1, 0.1);

  scene_ = new QGraphicsScene(this);
  setScene(scene_);

  map_ = new ItemMap();
  mario_ = new ItemMario();

  scene_->addItem(map_);
  scene_->addItem(mario_);

  setDragMode(ScrollHandDrag);

  refresh();
}

void MapGeneral::refresh() {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;
  map_->set_map();

  for (auto* manager : managers_) {
    scene_->removeItem(manager);
    delete manager;
  }
  managers_.clear();
  const u32 p_conductor = read_u32(0x8040a6e8);
  const s32 count = read_u32(p_conductor + 0x14);
  const u32 head = read_u32(p_conductor + 0x18);
  u32 next = head;
  for (s32 i = 0; i < count - 1; i++) {
    next = read_u32(next);
    const u32 p_manager = read_u32(next + 0x8);
    // クラス名によっては専用のItemManagerクラスを使う？
    auto* manager = new ItemManagerBase(p_manager, i);
    scene_->addItem(manager);
    managers_.append(manager);
  }

  // ここにmap_->stage_, map_->episode_ごとの処理？
}

void MapGeneral::timerEvent(QTimerEvent* event) {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  if (map_->is_changed())
    return;
  
  mario_->update();
  map_->update(mario_->y_);
  for (auto* manager : managers_) {
    manager->update();
  }

  if (center_on_mario_)
    centerOn(mario_->pix_);

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

void MapGeneral::mouseDoubleClickEvent(QMouseEvent* event) {
  auto* item = scene_->itemAt(mapToScene(event->pos()), QTransform());
  if (item->type() == OBJ) {
    const auto* obj = dynamic_cast<ItemObjBase*>(item);
    qDebug() << "Clicked on item:" << QString::fromStdString(obj->class_name_);
    qDebug() << obj->manager_id_ << "," << obj->id_;
  }
  QGraphicsView::mouseDoubleClickEvent(event);
}

void MapGeneral::set_center_on_mario(const bool is_center) {
  center_on_mario_ = is_center;
}

void MapGeneral::set_timer_interval(const s32 interval) {
  if (interval < 16)
    return
  killTimer(timer_id_);
  timer_id_ = startTimer(interval);
}
