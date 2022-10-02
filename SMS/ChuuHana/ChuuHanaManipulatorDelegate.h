#pragma once

#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QSpinBox>

#include "Common/CommonTypes.h"

class SpinBox7a5b : public QSpinBox {
  Q_OBJECT
public:
  SpinBox7a5b(QWidget* parent = nullptr) : QSpinBox(parent) {
    lineEdit()->setReadOnly(true);
  }
  void value7a5b(u32* a, u32* b) const {
    *a = a_;
    *b = b_;
  }
  void setValue7a5b(const u32 a, const u32 b) {
    setRange(0, a / 5 + b / 7);
    setValue(b / 7);
    a_ = a;
    b_ = b;
  }
  void stepBy(const int steps) override {
    a_ -= 5 * steps;
    b_ += 7 * steps;
    QSpinBox::stepBy(steps);
  }
protected:
  [[nodiscard]] QString textFromValue(const int val) const override {
    return "(" + QString::number(a_) + " " + QString::number(b_) + ")";
  }
private:
  u32 a_ = 0;
  u32 b_ = 0;
};

class ChuuHanaManipulatorDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  ChuuHanaManipulatorDelegate(QObject* parent = nullptr);
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};
