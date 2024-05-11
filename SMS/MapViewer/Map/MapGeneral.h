#pragma once

#include <QGraphicsView>
#include "Common/CommonTypes.h"
#include "Items.h"
#include "Memory/Memory.h"
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsRectItem>

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
  void select_item_by_address(u32 address);

signals:
  void map_object_clicked(u32 address);

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
  ItemObjBase* selected_obj_ = nullptr;
};
