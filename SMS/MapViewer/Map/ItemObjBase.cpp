#include "Items.h"

ItemObjBase::ItemObjBase(const u32 p_obj, ItemManagerBase* parent) 
  : QGraphicsItemGroup(parent), p_obj_(p_obj), manager_id_(parent->id_) {
  pix_ = new QGraphicsPixmapItem(this);

  rect_ = new QGraphicsRectItem(this);
  rect_->setPen(QPen(Qt::gray, 20));
  rect_->setVisible(false);

  addToGroup(pix_);
  addToGroup(rect_);
  setVisible(false);

  // TODO hitbox‚Ì‘å‚«‚³‚Ì•`‰æ on/off‚Å‚«‚é‚æ‚¤‚É
}

void ItemObjBase::update() {
  x_ = read_float(p_obj_ + 0x10);
  y_ = read_float(p_obj_ + 0x14);
  z_ = read_float(p_obj_ + 0x18);
  scale_ = read_float(p_obj_ + 0x24);
  rot_y_ = read_float(p_obj_ + 0x34);
  id_ = read_s16(p_obj_ + 0x7c);
  draw_info_ = read_u32(p_obj_ + 0xf0);

  set_appearance();
  set_scale();
  const QRectF bounds = pix_->boundingRect();
  pix_->setTransformOriginPoint(bounds.center());
  pix_->setTransform(QTransform().translate(-bounds.center().x(), -bounds.center().y()));
  pix_->setPos(x_, z_);
  set_rotation();
  setVisible(!(draw_info_ & 1));

  if (is_selected_) {
    rect_->setRect(0, 0,
      bounds.width() * pix_->scale(),
      bounds.height() * pix_->scale());
    rect_->setPos(pix_->pos() - bounds.center() * pix_->scale());
    rect_->setVisible(true);
  } else {
    rect_->setVisible(false);
  }
}

void ItemObjBase::set_scale() {
  pix_->setScale(scale_ * g_obj_scale);
}

void ItemObjBase::set_rotation() {
  pix_->setRotation(-rot_y_);
}

void ItemObjBase::set_appearance() {}
