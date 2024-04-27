#include "Items.h"

ItemObjBase::ItemObjBase(const u32 p_obj, const std::string class_name, const s32 manager_id) 
  : p_obj_(p_obj), class_name_(class_name), manager_id_(manager_id) {
  QPixmap pix;
  if (g_class_to_png.contains(class_name)) {
    const std::string dir = g_class_to_png[class_name];
    pix = QPixmap(QString::fromStdString(dir));
    pix_ = new QGraphicsPixmapItem(pix);
    addToGroup(pix_);
    pix_->setTransformOriginPoint(pix.width() / 2.0, pix.height() / 2.0);
    pix_->setTransform(QTransform().translate(-pix.width() / 2.0, -pix.height() / 2.0));
  } else {
    pix_ = new QGraphicsPixmapItem();
  }
  pix_->setVisible(false);
  // TODO hitbox‚Ì‘å‚«‚³‚Ì•`‰æ on/off‚Å‚«‚é‚æ‚¤‚É
}

void ItemObjBase::update() {
  x_ = read_float(p_obj_ + 0x10);
  y_ = read_float(p_obj_ + 0x14);
  z_ = read_float(p_obj_ + 0x18);
  rot_y_ = read_float(p_obj_ + 0x34);
  scale_ = read_float(p_obj_ + 0x24);
  draw_info_ = read_u32(p_obj_ + 0xf0);
  id_ = read_s16(p_obj_ + 0x7c);

  pix_->setScale(scale_ * g_obj_scale);
  const QRectF bounds = pix_->boundingRect();
  pix_->setTransformOriginPoint(bounds.center());
  pix_->setTransform(QTransform().translate(-bounds.center().x(), -bounds.center().y()));
  pix_->setPos(x_, z_);
  pix_->setRotation(-rot_y_);
  pix_->setVisible(!(draw_info_ & 1));
}