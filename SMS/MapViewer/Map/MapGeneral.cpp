#include "MapGeneral.h"

MapGeneral::MapGeneral(QWidget *parent)
  : QGraphicsView(parent) {
  timer_id_ = startTimer(33);
}

void MapGeneral::init() {
  scale(0.05, 0.05);

  scene_ = new QGraphicsScene(this);
  setScene(scene_);

  map_ = new ItemMap();
  mario_ = new ItemMario();

  //auto test = new QGraphicsEllipseItem(-100, -100, 200, 200, mario_);
  //QPen pen(Qt::red);
  //pen.setWidth(20);
  //test->setPen(pen);

  scene_->addItem(map_);
  scene_->addItem(mario_);

  //auto* test = new CustomPixmapItem();
  //test->setPixmap(QPixmap(":/sms/manta_purple.png"));
  //test->setScale(10);
  //const QRectF bounds = test->boundingRect();
  //test->setTransformOriginPoint(bounds.center());
  //test->setTransform(QTransform().translate(-bounds.center().x(), -bounds.center().y()));
  //test->setPos(1000, 1000);
  //scene_->addItem(test);

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

// TODO ItemそのものにmousePressEventを実装して、そこでemitする？
// TODO QGraphicsItem::ItemIsSelectable
// TODO QGraphicsScene::setSelectionArea
void MapGeneral::mousePressEvent(QMouseEvent* event) {
  auto* pix_item = scene_->itemAt(mapToScene(event->pos()), QTransform());
  auto* item = pix_item->parentItem();
  if (item != nullptr && item->type() == OBJ) {
    auto* obj = dynamic_cast<ItemObjBase*>(item);
    if (selected_obj_ == nullptr) {
      obj->is_selected_ = true;
      selected_obj_ = obj;
    }
    if (selected_obj_ != obj) {
      selected_obj_->is_selected_ = false;
      obj->is_selected_ = true;
      selected_obj_ = obj;
    }
    emit map_object_clicked(obj->p_obj_);
  }
  QGraphicsView::mousePressEvent(event);
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

void MapGeneral::select_item_by_address(const u32 address) {
  // get item by address
  if (address == mario_->p_mario_) {
    // TODO
    return;
  }
  for (const auto* manager : managers_) {
    for (auto* obj : manager->objs_) {
      if (address == obj->p_obj_) {
        // TODO
      }
    }
  }
}
