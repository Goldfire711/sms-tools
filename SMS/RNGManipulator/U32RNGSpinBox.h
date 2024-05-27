#pragma once
#include <QSpinBox>
#include <QWidget>
#include <QLineEdit>

#include "Common/CommonTypes.h"

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
