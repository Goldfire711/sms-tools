#include "MapObjectViewer.h"

#include <QHBoxLayout>

#include "DolphinProcess/Windows/WindowsDolphinProcess.h"


MapObjectViewer::MapObjectViewer(QWidget* parent)
  : QWidget(parent), tree_view_(new QTreeView()) {
  auto* lo_main = new QHBoxLayout();
  lo_main->addWidget(tree_view_);
  setLayout(lo_main);

  refresh();
}

void MapObjectViewer::refresh() {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  if (model_ != nullptr) {
    killTimer(timer_id_);
    tree_view_->setModel(nullptr);
    delete model_;
    model_ = nullptr;
  }
  model_ = new MapObjectViewerModel();
  auto* root_item = model_->root_item_.get();

  const u32 p_mario = read_u32(0x8040a378);
  if (p_mario == 0)
    return;
  const u32 p_camera = read_u32(0x8040b370);
  root_item->append_object(p_mario);
  root_item->append_object(p_camera);

  const u32 p_items = read_u32(0x8040a4d8);
  root_item->append_manager(p_items);

  const u32 p_conductor = read_u32(0x8040a6e8);
  const s32 count = read_s32(p_conductor + 0x14);
  const u32 head = read_u32(p_conductor + 0x18);
  u32 next = head;
  for (s32 i = 0; i < count - 1; i++) {
    next = read_u32(next);
    const u32 p_manager = read_u32(next + 0x8);
    root_item->append_manager(p_manager);
  }

  tree_view_->setModel(model_);
  tree_view_->resizeColumnToContents(0);
  tree_view_->resizeColumnToContents(1);

  timer_id_ = startTimer(timer_interval_);
}

void MapObjectViewer::timerEvent(QTimerEvent* event) {
  emit model_->layoutChanged();
}

void MapObjectViewer::set_timer_interval(s32 interval) {
  if (interval < 16)
    return;
  timer_interval_ = interval;
  killTimer(timer_id_);
  timer_id_ = startTimer(interval);
}
