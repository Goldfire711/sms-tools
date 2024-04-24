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
  void set_timer_interval(s32 interval);

  bool center_on_mario_ = false;

protected:
  void timerEvent(QTimerEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

private:
  virtual void init();

  QGraphicsScene* scene_;
  s32 timer_id_ = -1;
  ItemMap* map_;
  ItemMario* mario_;
};
