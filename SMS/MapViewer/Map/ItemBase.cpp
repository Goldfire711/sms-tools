#include "Items.h"

ItemBase::ItemBase(QGraphicsItem* parent)
  : QGraphicsItemGroup(parent),
    pix_(new QGraphicsPixmapItem(this)),
    rect_(new QGraphicsRectItem(this)),
    sub_items_(new std::vector<SubItem>){
  rect_->setPen(QPen(Qt::gray, 20));
  rect_->setVisible(false);

  addToGroup(pix_);
  addToGroup(rect_);
}
