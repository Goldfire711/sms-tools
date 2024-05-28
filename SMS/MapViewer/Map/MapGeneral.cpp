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
  // TODO item managerの追加

  scene_->addItem(map_);
  scene_->addItem(mario_);

  setDragMode(ScrollHandDrag);

  refresh();

  setContextMenuPolicy(Qt::CustomContextMenu);
}

void MapGeneral::refresh() {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;
  map_->set_map();
  emit map_changed(map_->maps_);

  QTransform map_transform = map_->transform();
  map_transform *= QTransform().scale(map_->scale() * 10, map_->scale() * 10); // TODO * 10を消す
  scene_->setSceneRect(map_transform.mapRect(map_->boundingRect()));

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

    const u32 vt = read_u32(p_manager);
    std::stringstream ss;
    ss << std::hex << vt;
    if (!g_vtable_to_class.contains(ss.str()))
      continue;
    const std::string obj_class = g_vtable_to_class[ss.str()];

    // クラス名によっては専用のItemManagerクラスを使う
    ItemManagerBase* manager;
    if (obj_class == "TBossMantaManager")
      manager = new ItemBossMantaManager(p_manager);
    else if (obj_class == "TNameKuriManager")
      manager = new ItemNameKuriManager(p_manager);
    else
      manager = new ItemManagerBase(p_manager);

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
  map_->update();
  for (auto* manager : managers_) {
    manager->update(map_->current_map_height_min_, map_->current_map_height_max_);
  }

  // TODO Center on機能を普通の敵にも使えるようにする(優先度低)
  // 右クリックのメニューからセットできるようにする
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

void MapGeneral::mousePressEvent(QMouseEvent* event) {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;
  auto pix_items = scene_->items(mapToScene(event->pos()));
  for (auto* pix_item : pix_items) {
    if (pix_item == nullptr)
      continue;
    auto* item = pix_item->parentItem();
    if (item != nullptr && (item->type() == ItemBase::OBJ
      || item->type() == ItemBase::MARIO)) {
      auto* obj = dynamic_cast<ItemBase*>(item);
      if (selected_obj_ == nullptr) {
        obj->is_selected_ = true;
        selected_obj_ = obj;
      }
      if (selected_obj_ != obj) {
        selected_obj_->is_selected_ = false;
        obj->is_selected_ = true;
        selected_obj_ = obj;
      }
      emit map_object_clicked(obj->ptr_);

      if (event->button() == Qt::RightButton) {
        show_context_menu(obj);
      }
      break;
    }
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

ItemBase* MapGeneral::select_item_by_address(const u32 address) {
  if (address == mario_->ptr_) {
    if (selected_obj_ != nullptr)
      selected_obj_->is_selected_ = false;
    mario_->is_selected_ = true;
    selected_obj_ = mario_;
    return mario_;
  }
  for (auto* manager : managers_) {
    // managersを取得した場合、selectはしない。
    if (address == manager->ptr_) {
      return manager;
    }
    for (auto* obj : manager->obj_list_) {
      if (address == obj->ptr_) {
        if (selected_obj_ != nullptr)
          selected_obj_->is_selected_ = false;
        obj->is_selected_ = true;
        selected_obj_ = obj;
        return obj;
      }
    }
  }
  return mario_;
}

void MapGeneral::set_map_layer(const s32 id) const {
  map_->selected_map_layer_ = id;
}

void MapGeneral::show_context_menu_by_address(const u32 address) {
  auto* item = select_item_by_address(address);
  show_context_menu(item);
}

void MapGeneral::show_context_menu(ItemBase* item) {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  // TODO moveTo関数の追加
  // TODO centerOn
  auto* menu = new QMenu(this);
  if (item->type() == ItemBase::MARIO || item->type() == ItemBase::MANAGER) {
    for (const auto& sub : *item->sub_items_) {
      auto* action = menu->addAction(QString("Show %1").arg(sub.name));
      action->setCheckable(true);
      action->setChecked(sub.item->isVisible());
      connect(action, &QAction::triggered, [sub](const bool checked) {
        sub.item->setVisible(checked);
        });
    }
  } else if (item->type() == ItemBase::OBJ) {
    auto* parent = dynamic_cast<ItemManagerBase*>(item->parentItem());

    // add actions for the manager of item
    for (const auto& sub : *parent->sub_items_) {
      auto* action = menu->addAction(QString("Show %1").arg(sub.name));
      action->setCheckable(true);
      action->setChecked(sub.item->isVisible());
      connect(action, &QAction::triggered, [sub](const bool checked) {
        sub.item->setVisible(checked);
        });
    }

    menu->addSeparator();

    // add actions for item
    const u32 vt = read_u32(item->ptr_);
    for (size_t i = 0; i < item->sub_items_->size(); i++) {
      auto* action = menu->addAction(QString("Show %1").arg(item->sub_items_->at(i).name));
      action->setCheckable(true);
      action->setChecked(item->sub_items_->at(i).item->isVisibleTo(item));
      connect(action, &QAction::triggered, [parent, i, vt](const bool checked) {
        for (auto* child : parent->obj_list_) {
          if (read_u32(child->ptr_) == vt) {
            child->sub_items_->at(i).item->setVisible(checked);
          }
        }
        });
    }
  }
  menu->exec(QCursor::pos());

}
