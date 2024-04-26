#include "Items.h"

ItemObjBase::ItemObjBase(const u32 p_obj, const std::string class_name) 
  : p_obj_(p_obj), class_name_(class_name) {
  QPixmap pix;
  if (g_class_to_png.contains(class_name)) {
    const std::string dir = g_class_to_png[class_name];
    pix = QPixmap(QString::fromStdString(dir));
    //pix = QPixmap(":/sms/puchipuchi_blue_x1.png");
    setPixmap(pix);
    setTransformOriginPoint(pix.width() / 2.0, pix.height() / 2.0);
    setTransform(QTransform().translate(-pix.width() / 2.0, -pix.height() / 2.0));
  }
  setVisible(false);
}

void ItemObjBase::update() {
  x_ = read_float(p_obj_ + 0x10);
  y_ = read_float(p_obj_ + 0x14);
  z_ = read_float(p_obj_ + 0x18);
  y_rot_ = read_float(p_obj_ + 0x34);
  scale_ = read_float(p_obj_ + 0x24);
  draw_info_ = read_u32(p_obj_ + 0xf0);

  setScale(scale_);
  setPos(x_, z_);
  setRotation(y_rot_);
  setVisible(!(draw_info_ & 1));
}