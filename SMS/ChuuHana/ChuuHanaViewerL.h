#pragma once

#include <QWidget>

class ChuuHanaViewerL : public QWidget {
  Q_OBJECT

public:
  ChuuHanaViewerL(QWidget* parent = nullptr);
  ~ChuuHanaViewerL() override;

protected:
  void paintEvent(QPaintEvent* event) override;

private:
};
