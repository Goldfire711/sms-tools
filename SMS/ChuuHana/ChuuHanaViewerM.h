#pragma once

#include <QWidget>

class ChuuHanaViewerM : public QWidget {
  Q_OBJECT

public:
  ChuuHanaViewerM(QWidget* parent);
  ~ChuuHanaViewerM() override;

protected:
  void paintEvent(QPaintEvent* event) override;
};
