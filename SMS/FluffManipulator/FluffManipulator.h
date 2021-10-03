#pragma once

#include "FluffManipulatorModel.h"
#include "FluffRNGFunctions.h"
#include "MapPianta8.h"
#include "SMSData.h"

#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTableView>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

class FluffManipulator : public QWidget
{
  Q_OBJECT

public:
  FluffManipulator(QWidget* parent, SMSData* sms_data);
  ~FluffManipulator();
  enum RadioButton
  {
    READ_FROM_RAM = 0,
    EDIT_RNGSEED,
    EDIT_RNGINDEX
  };

  enum ComboBox
  {
    RANGE_TYPE_MIDDLE = 0,
    RANGE_TYPE_BOTTOM,
    RANGE_TYPE_TOP
  };

private:

  void initialiseWidgets();
  void makeLayouts();
  void update();
  void updateRNGTextBox();
  void updateRangeX();
  void updateRangeY();
  void updateRangeZ();
  void onRngSeedChanged();
  void onRngIndexChanged();
  void onSwingDirectionChanged();
  void onSwingDirectionHexChanged();
  void onSearchOriginCheckBoxChanged();
  void show_widget_map_pianta_8();
  QLabel* m_lblRngSeed;
  QLabel* m_lblRngIndex;
  QRadioButton* m_rdbEditRngSeed;
  QRadioButton* m_rdbEditRngIndex;
  QRadioButton* m_rdbReadFromRAM;
  QButtonGroup* m_btnGroupEdit;
  QLineEdit* m_txbRngSeed;
  QLineEdit* m_txbRngIndex;
  QPushButton* m_updateButton;
  QTableView* m_tblFluffList;
  FluffManipulatorModel* m_fluffListModel;

  QLineEdit* m_txbTargetX;
  QLineEdit* m_txbTargetY;
  QLineEdit* m_txbTargetZ;

  QLineEdit* m_txbTargetSwingDirection;
  QLineEdit* m_txbTargetSwingDirectionHex;

  QLineEdit* m_txbRangeX;
  QLineEdit* m_txbRangeY;
  QLineEdit* m_txbRangeZ;
  QComboBox* m_cmbRangeXType;
  QComboBox* m_cmbRangeYType;
  QComboBox* m_cmbRangeZType;

  QLabel* m_lblRangeMinX;
  QLabel* m_lblRangeMaxX;
  QLabel* m_lblRangeMinY;
  QLabel* m_lblRangeMaxY;
  QLabel* m_lblRangeMinZ;
  QLabel* m_lblRangeMaxZ;

  u32 m_currentRngSeed;
  u32 m_currentRngIndex;
  u32 m_editedRngSeed;
  u32 m_editedRngIndex;

  QLineEdit* m_txbSearchRange;
  QCheckBox* m_chbSearchOrigin;

  QLabel* m_lblElapsedTime;

  SMSData* m_smsData;

  QLabel* m_lblProbability;

  MapPianta8* map_pianta_8_ = nullptr;
  QPushButton* m_btnShowMap;
};