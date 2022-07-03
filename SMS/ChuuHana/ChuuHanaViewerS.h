#pragma once

#include <QWidget>

class ChuuHanaViewerS : public QWidget {
  Q_OBJECT

public:
  ChuuHanaViewerS(QWidget* parent);
  ~ChuuHanaViewerS() override;

protected:
  void paintEvent(QPaintEvent* event) override;
};
