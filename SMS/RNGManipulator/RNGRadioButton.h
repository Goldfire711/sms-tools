#pragma once
#include <QRadioButton>
#include <QWidget>
#include <QLineEdit>
#include <QButtonGroup>
#include <QLabel>

#include "Common/CommonTypes.h"
#include "U32RNGSpinBox.h"

class ClickableLabel : public QLabel {
  Q_OBJECT
public:
  explicit ClickableLabel(const QString &text, QWidget *parent = nullptr) : QLabel(text, parent) {}
signals:
  void clicked();
protected:
  void mousePressEvent(QMouseEvent* event) override {
    emit clicked();
  }
};

class RNGRadioButton : public QWidget {
  Q_OBJECT

public:
  RNGRadioButton(QWidget* parent);
  u32 get_seed();

  enum RadioButton {
    READ_FROM_RAM = 0,
    EDIT_RNG
  };

private:
  void on_rdb_clicked(s32 id) const;
  void on_rng_seed_changed(const QString& str_seed);
  void on_rng_index_changed();
  u32 ram_seed_ = 0;
  u32 ram_index_ = 0;
  u32 edited_seed_ = 0;
  u32 edited_index_ = 0;

  QRadioButton* rdb_read_from_ram_;
  QRadioButton* rdb_edit_rng_;
  QButtonGroup* grp_rdb_;

  QLineEdit* txb_rng_seed_;
  U32RNGSpinBox* spb_rng_index_;
};
