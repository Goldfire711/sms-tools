#pragma once

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QSpinBox>
#include <QDebug>

#include "U32RNGSpinBox.h"
#include "Common/CommonTypes.h"

class RNGViewer : public QWidget {
  Q_OBJECT

public:
  RNGViewer(QWidget* parent);
  void update_ram_rng();

private:
  void on_edit_seed_changed();
  void on_edit_index_changed();
  void on_chb_edit_changed(s32 state) const;
  QLabel* lbl_seed_;
  QLabel* lbl_index_;
  QLineEdit* txb_seed_;
  QLineEdit* txb_index_;
  QCheckBox* chb_edit_;
  QPushButton* btn_write_;
  QPushButton* btn_read_;
  QLabel* lbl_edit_seed_;
  QLabel* lbl_edit_index_;
  QLineEdit* txb_edit_seed_;
  U32RNGSpinBox* spb_edit_index_;
  QLabel* lbl_index_diff_;
  QLabel* lbl_index_diff2_;

  u32 seed_ = 0;
  u32 index_ = 0;
  u32 edit_seed_ = 0;
  u32 edit_index_ = 0;
};