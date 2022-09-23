#pragma once

#include <QWidget>
#include <QElapsedTimer>

#include "Common/CommonTypes.h"

class ChuuHanaViewerL : public QWidget {
  Q_OBJECT

public:
  ChuuHanaViewerL(QWidget* parent = nullptr);
  ~ChuuHanaViewerL() override;

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  struct ChuuHana {
    s64 elapsed_time_target_changed = 0;
    enum {
      WILL_FALL,
      SET_GOAL,
      COLLID_MOVE
    } target_type = WILL_FALL;
  };
  ChuuHana chuuhana_[6];
  QElapsedTimer elapsed_timer_;
};
