#pragma once

#include <QGraphicsView>
#include "Common/CommonTypes.h"
#include "Items.h"
#include "Memory/Memory.h"
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsRectItem>
#include <QMenu>

using namespace memory;

class MapGeneral  : public QGraphicsView {
  Q_OBJECT

public:
  MapGeneral(QWidget *parent);
  virtual void init();
  virtual void refresh();
  void set_center_on_mario(bool is_center);
  void set_timer_interval(s32 interval);

  bool center_on_mario_ = false;
  s32 timer_id_ = -1;

public slots:
  ItemBase* select_item_by_address(u32 address);
  void set_map_layer(s32 id) const;
  void show_context_menu_by_address(u32 address);
  void show_context_menu(ItemBase* item);

signals:
  void map_object_clicked(u32 address);
  void map_changed(QVector<ItemMap::map_height> maps);

protected:
  void timerEvent(QTimerEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  //void mouseDoubleClickEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

private:
  QGraphicsScene* scene_;
  ItemMap* map_;
  ItemMario* mario_;
  QVector<ItemManagerBase*> managers_;
  ItemBase* selected_obj_ = nullptr;
};
