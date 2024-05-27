#pragma once

#include <QWidget>
#include "RNG.h"
#include "RNGManipulatorModel.h"
#include "RNGViewer.h"
#include "RNGManipulatorDelegate.h"

#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTableView>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>

#include "Common/CommonTypes.h"
#include "RNGRadioButton.h"

class RNGManipulator : public QWidget {
  Q_OBJECT

public:
  RNGManipulator(QWidget* parent = nullptr);

private:
  void initialize_widgets();
  void make_layouts();
  void search() const;
  void on_context_menu_requested(const QPoint& pos);
  void copy_selection() const;

  RNGRadioButton* rdb_rng_;
  QPushButton* btn_search_;
  RNGManipulatorModel* model_tbl_rng_;;
  QTableView* tbl_rng_;
  QLineEdit* txb_rng_from_;
  QLineEdit* txb_rng_to_;
  QLineEdit* txb_search_from_;
  QLineEdit* txb_search_to_;
  QLineEdit* txb_search_range_;
  QLabel* lbl_elapsed_time_;
  QLabel* lbl_probability_;
  QGroupBox* group_setting_;

  RNG* rng_;

  RNGViewer* rng_viewer_ = nullptr;

  s32 select_search_ = 0;

  RNGManipulatorDelegate* delegate_;
};
