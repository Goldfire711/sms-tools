#pragma once

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QSpinBox>
#include <QDebug>

#include "Common/CommonTypes.h"

//test
class U32RNGSpinBox : public QSpinBox {
public:
  U32RNGSpinBox(QWidget* parent = nullptr) : QSpinBox(parent) {
    setRange(INT_MIN, INT_MAX);
  }
  [[nodiscard]] u32 valueU32() const {
    return u(value());
  }
  void setValueU32(const u32 value) {
    setValue(s(value));
  }
  void stepBy(int steps) override {
    QSpinBox::stepBy(steps);
    lineEdit()->setSelection(lineEdit()->selectionLength(), 0);
  }

protected:
  [[nodiscard]] QString textFromValue(const int val) const override {
    return QString::number(u(val));
  }
  [[nodiscard]] int valueFromText(const QString& text) const override {
    return s(text.toUInt());
  }
  QValidator::State validate(QString& input, int& pos) const override {
    const QRegExp reg_uint(
      R"(^(0|(\+)?[1-9]{1}[0-9]{0,8}|(\+)?[1-3]{1}[0-9]{1,9}|(\+)?[4]{1}([0-1]{1}[0-9]{8}|[2]{1}([0-8]{1}[0-9]{7}|[9]{1}([0-3]{1}[0-9]{6}|[4]{1}([0-8]{1}[0-9]{5}|[9]{1}([0-5]{1}[0-9]{4}|[6]{1}([0-6]{1}[0-9]{3}|[7]{1}([0-1]{1}[0-9]{2}|[2]{1}([0-8]{1}[0-9]{1}|[9]{1}[0-5]{1})))))))))$)");
    const auto val_uint = QRegExpValidator(reg_uint);
    return val_uint.validate(input, pos);
  }

private:
  [[nodiscard]] u32 u(s32 s) const {
    return *reinterpret_cast<u32*>(&s);
  }
  [[nodiscard]] s32 s(u32 u) const {
    return *reinterpret_cast<s32*>(&u);
  }
};

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