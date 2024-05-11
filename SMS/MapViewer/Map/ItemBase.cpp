#include "Items.h"

ItemBase::ItemBase(QGraphicsItem* parent)
  : QGraphicsItemGroup(parent),
    pix_(new QGraphicsPixmapItem(this)),
    rect_(new QGraphicsRectItem(this)) {
  rect_->setPen(QPen(Qt::gray, 20));
  rect_->setVisible(false);

  addToGroup(pix_);
  addToGroup(rect_);
}
