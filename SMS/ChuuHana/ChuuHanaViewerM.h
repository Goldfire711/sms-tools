#pragma once

#include <QElapsedTimer>
#include <QWidget>

#include "Common/CommonTypes.h"

class ChuuHanaViewerM : public QWidget {
  Q_OBJECT

public:
  ChuuHanaViewerM(QWidget* parent = nullptr);
  ~ChuuHanaViewerM() override;

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
