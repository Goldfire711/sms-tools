#pragma once

#include <QGraphicsView>
#include "../../Common/CommonTypes.h"
#include "Items.h"
#include "../../Memory/Memory.h"
#include <QMouseEvent>

using namespace memory;

class MapGeneral  : public QGraphicsView {
  Q_OBJECT

public:
  MapGeneral(QWidget *parent);
  virtual void refresh();
  void set_center_on_mario(const bool is_center);
  void set_timer_interval(const s32 interval);

  bool center_on_mario_ = false;
  s32 timer_id_ = -1;

protected:
  void timerEvent(QTimerEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

private:
  virtual void init();

  QGraphicsScene* scene_;
  ItemMap* map_;
  ItemMario* mario_;
  QVector<ItemGroupManagerBase*> managers_;
};
