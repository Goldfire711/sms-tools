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

class RNGManipulator : public QWidget {
  Q_OBJECT

public:
  RNGManipulator(QWidget* parent = nullptr);
  enum RadioButtonRNG {
    READ_FROM_RAM = 0,
    EDIT_RNGSEED,
    EDIT_RNGINDEX
  };

  enum RadioButtonType {
    TYPE_AUTO = 0,
    TYPE_SET_GOAL,
    TYPE_NODE
  };

  enum ComboBoxNodeRange {
    ZERO_TO_SIX = 0,
    ZERO_TO_SEVEN
  };

private:
  void initialize_widgets();
  void make_layouts();
  void update();
  void update_rng_textbox();
  void on_rng_seed_changed();
  void on_rng_index_changed();
  void on_context_menu_requested(const QPoint& pos);
  void copy_selection() const;
  QLabel* lbl_rng_seed_;
  QLabel* lbl_rng_index_;
  QRadioButton* rdb_read_from_ram_;
  QRadioButton* rdb_edit_rng_seed_;
  QRadioButton* rdb_edit_rng_index_;
  QButtonGroup* group_rdb_rng_;
  QLineEdit* txb_rng_seed_;
  QLineEdit* txb_rng_index_;
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

  u32 ram_seed_ = 0;
  u32 ram_index_ = 0;
  u32 edited_seed_ = 0;
  u32 edited_index_ = 0;
  RNG* rng_;

  RNGViewer* rng_viewer_ = nullptr;

  s32 select_search_ = 0;
  s32 select_ = 0;

  RNGManipulatorDelegate* delegate_;
};
