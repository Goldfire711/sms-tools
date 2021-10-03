#include "FluffManipulator.h"

#include "../../DolphinProcess/DolphinAccessor.h"
#include "../../Memory/Memory.h"
#include "RNGFunctions.h"


#include <iostream>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QValidator>
#include <QRegExp>
#include <QCoreApplication>
#include <sstream>
#include <chrono>

FluffManipulator::FluffManipulator(QWidget* parent, SMSData* sms_data) : QWidget(parent), m_smsData(sms_data)
{
  setWindowTitle(tr("Fluff Manipulator"));
  //resize(400, 400);
  initialiseWidgets();
  makeLayouts();
  update();
  m_editedRngSeed = m_currentRngSeed;
  m_editedRngIndex = m_currentRngIndex;
  updateRNGTextBox();

  if (!map_pianta_8) {
    map_pianta_8 = new MapPianta8(nullptr, m_smsData);
  }
  map_pianta_8->show();
  map_pianta_8->raise();
  map_pianta_8->activateWindow();
}

FluffManipulator::~FluffManipulator()
{
  map_pianta_8->close();
}

void FluffManipulator::initialiseWidgets()
{
  // updateボタン
  m_updateButton = new QPushButton(tr("update"), this);
  connect(m_updateButton, &QPushButton::clicked, this, &FluffManipulator::update);

  // SeedをRAMから読み込むか、Seed,Indexを手動で編集するか選択するラジオボタン
  m_rdbReadFromRAM = new QRadioButton(tr("Read RNG Seed from RAM"));
  m_rdbEditRngSeed = new QRadioButton(tr("RNG Seed: 0x"));
  m_rdbEditRngIndex = new QRadioButton(tr("RNG Index: "));

  m_btnGroupEdit = new QButtonGroup(this);
  m_btnGroupEdit->addButton(m_rdbReadFromRAM, RadioButton::READ_FROM_RAM);
  m_btnGroupEdit->addButton(m_rdbEditRngSeed, RadioButton::EDIT_RNGSEED);
  m_btnGroupEdit->addButton(m_rdbEditRngIndex, RadioButton::EDIT_RNGINDEX);
  m_rdbReadFromRAM->setChecked(true);
  connect(m_btnGroupEdit, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this,
    &FluffManipulator::updateRNGTextBox);

  // Seed値・Index値のlineEdit
  m_txbRngSeed = new QLineEdit("FFFFFFFF");
  m_txbRngIndex = new QLineEdit("4294967295");
  m_txbRngSeed->setFixedWidth(120);
  m_txbRngIndex->setFixedWidth(120);
  const QRegExp rxHex32("[0-9a-fA-F]{1,8}");
  const QRegExp rxRngIndex(
    R"(^(0|(\+)?[1-9]{1}[0-9]{0,8}|(\+)?[1-3]{1}[0-9]{1,9}|(\+)?[4]{1}([0-1]{1}[0-9]{8}|[2]{1}([0-8]{1}[0-9]{7}|[9]{1}([0-3]{1}[0-9]{6}|[4]{1}([0-8]{1}[0-9]{5}|[9]{1}([0-5]{1}[0-9]{4}|[6]{1}([0-6]{1}[0-9]{3}|[7]{1}([0-1]{1}[0-9]{2}|[2]{1}([0-8]{1}[0-9]{1}|[9]{1}[0-5]{1})))))))))$)");
  QRegExpValidator* validHex32 = new QRegExpValidator(rxHex32, this);
  QRegExpValidator* validRngIndex = new QRegExpValidator(rxRngIndex, this);
  m_txbRngSeed->setValidator(validHex32);
  m_txbRngIndex->setValidator(validRngIndex);
  connect(m_txbRngSeed, &QLineEdit::textEdited, this, &FluffManipulator::onRngSeedChanged);
  connect(m_txbRngIndex, &QLineEdit::textEdited, this, &FluffManipulator::onRngIndexChanged);

  // update();
  // updateRNGTextBox();

  // 綿毛生成座標のターゲット(XYZ)と範囲
  m_txbTargetX = new QLineEdit("2815");
  m_txbTargetY = new QLineEdit("9139");
  m_txbTargetZ = new QLineEdit("-635");
  m_txbTargetX->setFixedWidth(60);
  m_txbTargetY->setFixedWidth(60);
  m_txbTargetZ->setFixedWidth(60);
  float amplitudeX = 425;  // 綿毛のX方向の最大振幅425
  float amplitudeZ = 425;  // 綿毛のY方向の最大振幅425
  m_smsData->m_bounds.min.x = -5000 - amplitudeX;
  m_smsData->m_bounds.max.x = 5000 + amplitudeX;
  m_smsData->m_bounds.min.y = 0;
  m_smsData->m_bounds.max.y = 9139;
  m_smsData->m_bounds.min.z = -5000 - amplitudeZ;
  m_smsData->m_bounds.max.z = 5000 + amplitudeZ;
  m_txbTargetX->setValidator(new QIntValidator(m_smsData->m_bounds.min.x, m_smsData->m_bounds.max.x, this));
  m_txbTargetY->setValidator(new QIntValidator(m_smsData->m_bounds.min.y, m_smsData->m_bounds.max.y, this));
  m_txbTargetZ->setValidator(new QIntValidator(m_smsData->m_bounds.min.z, m_smsData->m_bounds.max.z, this));
  m_txbTargetX->setToolTip(QString::fromStdString(
    std::to_string(m_smsData->m_bounds.min.x) + " .. " + std::to_string(m_smsData->m_bounds.max.x)));
  m_txbTargetY->setToolTip(QString::fromStdString(
    std::to_string(m_smsData->m_bounds.min.y) + " .. " + std::to_string(m_smsData->m_bounds.max.y)));
  m_txbTargetZ->setToolTip(QString::fromStdString(
    std::to_string(m_smsData->m_bounds.min.z) + " .. " + std::to_string(m_smsData->m_bounds.max.z)));
  // connect(m_txbTargetX, &QLineEdit::textChanged, this, &FluffManipulator::updateRangeX);
  // connect(m_txbTargetY, &QLineEdit::textChanged, this, &FluffManipulator::updateRangeY);
  // connect(m_txbTargetZ, &QLineEdit::textChanged, this, &FluffManipulator::updateRangeZ);

  // 綿毛の揺れる方向(TargetのGroupBox内)
  m_txbTargetSwingDirection = new QLineEdit("0");
  m_txbTargetSwingDirection->setFixedWidth(100);
  m_txbTargetSwingDirection->setValidator(new QDoubleValidator(0, 360, 1000, this));
  m_txbTargetSwingDirection->setToolTip(QString::fromStdString("[0, 360]"));
  m_txbTargetSwingDirectionHex = new QLineEdit("0");
  m_txbTargetSwingDirectionHex->setFixedWidth(100);
  m_txbTargetSwingDirectionHex->setValidator(validHex32);
  connect(m_txbTargetSwingDirection, &QLineEdit::textEdited, this, &FluffManipulator::onSwingDirectionChanged);
  connect(m_txbTargetSwingDirectionHex, &QLineEdit::textEdited, this, &FluffManipulator::onSwingDirectionHexChanged);

  // 範囲の指定
  m_txbRangeX = new QLineEdit("1000");
  m_txbRangeY = new QLineEdit("1000");
  m_txbRangeZ = new QLineEdit("1000");
  m_txbRangeX->setFixedWidth(60);
  m_txbRangeY->setFixedWidth(60);
  m_txbRangeZ->setFixedWidth(60);
  m_txbRangeX->setValidator(new QIntValidator(0, 10000, this));
  m_txbRangeY->setValidator(new QIntValidator(0, 10000, this));
  m_txbRangeZ->setValidator(new QIntValidator(0, 10000, this));
  // connect(m_txbRangeX, &QLineEdit::textChanged, this, &FluffManipulator::updateRangeX);
  // connect(m_txbRangeY, &QLineEdit::textChanged, this, &FluffManipulator::updateRangeY);
  // connect(m_txbRangeZ, &QLineEdit::textChanged, this, &FluffManipulator::updateRangeZ);


  m_cmbRangeXType = new QComboBox();
  m_cmbRangeYType = new QComboBox();
  m_cmbRangeZType = new QComboBox();
  QStringList rangeTypeList = { QString::fromLocal8Bit("←・→"), QString::fromLocal8Bit("・→"),
                               QString::fromLocal8Bit("←・") };
  m_cmbRangeXType->addItems(rangeTypeList);
  m_cmbRangeYType->addItems(rangeTypeList);
  m_cmbRangeZType->addItems(rangeTypeList);
  m_cmbRangeYType->setCurrentIndex(2);
  // connect(m_cmbRangeXType, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
  //         &FluffManipulator::updateRangeX);
  // connect(m_cmbRangeYType, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
  //         &FluffManipulator::updateRangeY);
  // connect(m_cmbRangeZType, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
  //         &FluffManipulator::updateRangeZ);

  m_lblRangeMinX = new QLabel("0000");
  m_lblRangeMaxX = new QLabel("0000");
  m_lblRangeMinY = new QLabel("0000");
  m_lblRangeMaxY = new QLabel("0000");
  m_lblRangeMinZ = new QLabel("0000");
  m_lblRangeMaxZ = new QLabel("0000");
  m_lblRangeMinX->setFixedWidth(50);
  m_lblRangeMaxX->setFixedWidth(50);
  m_lblRangeMinY->setFixedWidth(50);
  m_lblRangeMaxY->setFixedWidth(50);
  m_lblRangeMinZ->setFixedWidth(50);
  m_lblRangeMaxZ->setFixedWidth(50);


  // 指定した範囲内に発生する綿毛とそのRNGの一覧
  //m_fluffListModel = new FluffManipulatorModel(this, &m_fluffsResult);
  m_fluffListModel = new FluffManipulatorModel(this, &m_smsData->m_fluffsResult);

  m_tblFluffList = new QTableView();
  m_tblFluffList->setModel(m_fluffListModel);
  m_tblFluffList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  m_tblFluffList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  // seedの検索範囲
  m_txbSearchRange = new QLineEdit("100000");
  m_txbSearchRange->setValidator(new QIntValidator(0, 10000000, this));
  m_txbSearchRange->setFixedWidth(100);

  // 位相を考慮するチェックボックス
  m_chbSearchOrigin = new QCheckBox("Search for x/z origin");
  //m_chbSearchOrigin = new QCheckBox(tr("Search for origin"));
  m_chbSearchOrigin->setCheckState(Qt::Unchecked);
  connect(m_chbSearchOrigin, &QCheckBox::stateChanged, this,
    &FluffManipulator::onSearchOriginCheckBoxChanged);

  // 処理時間
  m_lblElapsedTime = new QLabel();

  // エリア内に綿毛が出現する確率（大雑把）
  m_lblProbability = new QLabel();
}

void FluffManipulator::makeLayouts()
{
  // QFormLayout* formEdit = new QFormLayout;
  // formEdit->setHorizontalSpacing(0);
  // formEdit->addRow(m_rdbEditRngSeed, m_txbRngSeed);
  // formEdit->addRow(m_rdbEditRngIndex, m_txbRngIndex);

  QHBoxLayout* editRngSeed_layout = new QHBoxLayout();
  editRngSeed_layout->setSpacing(0);
  editRngSeed_layout->addWidget(m_rdbEditRngSeed);
  editRngSeed_layout->addWidget(m_txbRngSeed);
  //editRngSeed_layout->addStretch(0);

  QHBoxLayout* editRngIndex_layout = new QHBoxLayout();
  editRngIndex_layout->setSpacing(0);
  editRngIndex_layout->addWidget(m_rdbEditRngIndex);
  editRngIndex_layout->addWidget(m_txbRngIndex);
  //editRngIndex_layout->addStretch(1);

  QVBoxLayout* btnGroupEdit_layout = new QVBoxLayout();
  btnGroupEdit_layout->addWidget(m_rdbReadFromRAM);
  btnGroupEdit_layout->addLayout(editRngSeed_layout);
  btnGroupEdit_layout->addLayout(editRngIndex_layout);

  QHBoxLayout* target_xyz_layout = new QHBoxLayout();
  target_xyz_layout->addWidget(new QLabel("X"));
  target_xyz_layout->addWidget(m_txbTargetX);
  target_xyz_layout->addStretch(0);
  target_xyz_layout->addWidget(new QLabel("Y"));
  target_xyz_layout->addWidget(m_txbTargetY);
  target_xyz_layout->addStretch(0);
  target_xyz_layout->addWidget(new QLabel("Z"));
  target_xyz_layout->addWidget(m_txbTargetZ);

  QHBoxLayout* target_swing_direction_layout = new QHBoxLayout();
  target_swing_direction_layout->addWidget(new QLabel("Swing direction"));
  target_swing_direction_layout->addWidget(m_txbTargetSwingDirection);
  target_swing_direction_layout->addWidget(new QLabel("Hex"));
  target_swing_direction_layout->addWidget(m_txbTargetSwingDirectionHex);

  QVBoxLayout* target_layout = new QVBoxLayout();
  target_layout->addLayout(target_xyz_layout);
  target_layout->addLayout(target_swing_direction_layout);

  QGroupBox* groupTarget = new QGroupBox(tr("Target"));
  groupTarget->setLayout(target_layout);

  QHBoxLayout* rangeX_layout = new QHBoxLayout();
  rangeX_layout->addWidget(new QLabel("X"));
  rangeX_layout->addWidget(new QLabel("min:"));
  rangeX_layout->addWidget(m_lblRangeMinX);
  rangeX_layout->addWidget(new QLabel("max:"));
  rangeX_layout->addWidget(m_lblRangeMaxX);
  rangeX_layout->addWidget(m_cmbRangeXType);
  rangeX_layout->addWidget(m_txbRangeX);

  QHBoxLayout* rangeY_layout = new QHBoxLayout();
  rangeY_layout->addWidget(new QLabel("Y"));
  rangeY_layout->addWidget(new QLabel("min:"));
  rangeY_layout->addWidget(m_lblRangeMinY);
  rangeY_layout->addWidget(new QLabel("max:"));
  rangeY_layout->addWidget(m_lblRangeMaxY);
  rangeY_layout->addWidget(m_cmbRangeYType);
  rangeY_layout->addWidget(m_txbRangeY);

  QHBoxLayout* rangeZ_layout = new QHBoxLayout();
  rangeZ_layout->addWidget(new QLabel("Z"));
  rangeZ_layout->addWidget(new QLabel("min:"));
  rangeZ_layout->addWidget(m_lblRangeMinZ);
  rangeZ_layout->addWidget(new QLabel("max:"));
  rangeZ_layout->addWidget(m_lblRangeMaxZ);
  rangeZ_layout->addWidget(m_cmbRangeZType);
  rangeZ_layout->addWidget(m_txbRangeZ);

  QVBoxLayout* range_layout = new QVBoxLayout();
  range_layout->addLayout(rangeX_layout);
  range_layout->addLayout(rangeY_layout);
  range_layout->addLayout(rangeZ_layout);

  QGroupBox* groupRange = new QGroupBox(tr("Range"));
  groupRange->setLayout(range_layout);

  QVBoxLayout* groupTargetRange_layout = new QVBoxLayout();
  groupTargetRange_layout->addWidget(groupTarget);
  groupTargetRange_layout->addWidget(groupRange);

  QHBoxLayout* RNGInformation_layout = new QHBoxLayout();
  RNGInformation_layout->addLayout(btnGroupEdit_layout);
  RNGInformation_layout->addStretch(1);
  RNGInformation_layout->addLayout(groupTargetRange_layout);

  QHBoxLayout* searchRange_layout = new QHBoxLayout();
  searchRange_layout->addWidget(m_lblProbability);
  searchRange_layout->addStretch(0);
  searchRange_layout->addWidget(m_chbSearchOrigin);
  searchRange_layout->addWidget(new QLabel("Search range"));
  searchRange_layout->addWidget(m_txbSearchRange);

  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_updateButton);
  mainLayout->addLayout(RNGInformation_layout);
  mainLayout->addLayout(searchRange_layout);
  mainLayout->addWidget(m_tblFluffList);
  mainLayout->addWidget(m_lblElapsedTime);

  setLayout(mainLayout);
}

void FluffManipulator::update()
{
  const auto start = std::chrono::steady_clock::now();

  updateRangeX();
  updateRangeY();
  updateRangeZ();

  u32 rngSeed = 0;
  switch (m_btnGroupEdit->checkedId())
  {
  case RadioButton::READ_FROM_RAM:
    if (DolphinComm::DolphinAccessor::getStatus() ==
      DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    {
      rngSeed = memory::read_u32(0x80408cf0);
      m_currentRngSeed = rngSeed;
      m_currentRngIndex = rng_functions::seed_to_index(rngSeed);
    }
    else
    {
      rngSeed = m_currentRngSeed;
    }

    break;
  default:
    rngSeed = m_editedRngSeed;
    break;
  }
  updateRNGTextBox();

  u32 searchRange = 100000;
  std::stringstream ss(m_txbSearchRange->text().toStdString());
  ss >> std::dec >> searchRange;

  m_smsData->search_for_origin = m_chbSearchOrigin->checkState();
  m_smsData->m_fluffsResult = fluff_rng_functions::searchFluffs(rngSeed, m_smsData->m_range, m_smsData->m_targetSwingDirection, m_smsData->m_target, searchRange, m_chbSearchOrigin->checkState());
  m_fluffListModel->updateList();

  const auto end = std::chrono::steady_clock::now();
  const std::string elapsedTime =
    std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + "ms";

  const std::string stringResultCount =
    std::to_string(m_smsData->m_fluffsResult.size()) + " result" + ((m_smsData->m_fluffsResult.size() < 2) ? "" : "s") + " found";

  const std::string resultInformation = stringResultCount + " (" + elapsedTime + ")";

  m_lblElapsedTime->setText(QString::fromStdString(resultInformation));

  const auto probability = static_cast<int64_t>(
    (10000.0 * 9139.0 * 10000.0) / ((m_smsData->m_range.max.x - m_smsData->m_range.min.x) *
      (m_smsData->m_range.max.y - m_smsData->m_range.min.y) *
      (m_smsData->m_range.max.z - m_smsData->m_range.min.z)));
  m_lblProbability->setText(tr("1/") + QString::number(probability));
}

void FluffManipulator::updateRNGTextBox()
{
  switch (m_btnGroupEdit->checkedId())
  {
  case RadioButton::READ_FROM_RAM:
    m_txbRngSeed->setText(QString::number(m_currentRngSeed, 16).toUpper());
    m_txbRngSeed->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0); border: none }");
    m_txbRngSeed->setReadOnly(true);
    m_txbRngIndex->setText(QString::number(m_currentRngIndex));
    m_txbRngIndex->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0); border: none }");
    m_txbRngIndex->setReadOnly(true);

    break;
  case RadioButton::EDIT_RNGSEED:
    m_txbRngSeed->setText(QString::number(m_editedRngSeed, 16).toUpper());
    m_txbRngSeed->setStyleSheet("");
    m_txbRngSeed->setReadOnly(false);
    m_txbRngIndex->setText(QString::number(m_editedRngIndex));
    m_txbRngIndex->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0); border: none }");
    m_txbRngIndex->setReadOnly(true);

    break;
  case RadioButton::EDIT_RNGINDEX:
    m_txbRngSeed->setText(QString::number(m_editedRngSeed, 16).toUpper());
    m_txbRngSeed->setStyleSheet("QLineEdit { background-color: rgba(0, 0, 0, 0); border: none }");
    m_txbRngSeed->setReadOnly(true);
    m_txbRngIndex->setText(QString::number(m_editedRngIndex));
    m_txbRngIndex->setStyleSheet("");
    m_txbRngIndex->setReadOnly(false);

    break;
  default:
    break;
  }
}

void FluffManipulator::updateRangeX()
{
  float rangeX = 0;

  std::stringstream ss(m_txbTargetX->text().toStdString());
  ss >> std::dec;
  ss >> m_smsData->m_target.x;
  std::stringstream ss2(m_txbRangeX->text().toStdString());
  ss2 >> std::dec;
  ss2 >> rangeX;

  if (!ss.fail() && !ss2.fail())
  {
    switch (m_cmbRangeXType->currentIndex())
    {
    case ComboBox::RANGE_TYPE_MIDDLE:
      m_smsData->m_range.min.x = m_smsData->m_target.x - rangeX / 2;
      m_smsData->m_range.max.x = m_smsData->m_target.x + rangeX / 2;
      break;
    case ComboBox::RANGE_TYPE_BOTTOM:
      m_smsData->m_range.min.x = m_smsData->m_target.x;
      m_smsData->m_range.max.x = m_smsData->m_target.x + rangeX;
      break;
    case ComboBox::RANGE_TYPE_TOP:
      m_smsData->m_range.min.x = m_smsData->m_target.x - rangeX;
      m_smsData->m_range.max.x = m_smsData->m_target.x;
      break;
    }

    if (m_smsData->m_range.min.x <= m_smsData->m_bounds.min.x)
    {
      m_smsData->m_range.min.x = m_smsData->m_bounds.min.x;
      m_lblRangeMinX->setStyleSheet("QLabel { color : red; }");
    }
    else
    {
      m_lblRangeMinX->setStyleSheet("QLabel { color : black; }");
    }
    if (m_smsData->m_range.max.x >= m_smsData->m_bounds.max.x)
    {
      m_smsData->m_range.max.x = m_smsData->m_bounds.max.x;
      m_lblRangeMaxX->setStyleSheet("QLabel { color : red; }");
    }
    else
    {
      m_lblRangeMaxX->setStyleSheet("QLabel { color : black; }");
    }
    m_lblRangeMinX->setText(QString::number(m_smsData->m_range.min.x));
    m_lblRangeMaxX->setText(QString::number(m_smsData->m_range.max.x));
  }
}

void FluffManipulator::updateRangeY()
{
  float rangeY = 0;

  std::stringstream ss(m_txbTargetY->text().toStdString());
  ss >> std::dec;
  ss >> m_smsData->m_target.y;
  std::stringstream ss2(m_txbRangeY->text().toStdString());
  ss2 >> std::dec;
  ss2 >> rangeY;

  if (!ss.fail() && !ss2.fail())
  {
    switch (m_cmbRangeYType->currentIndex())
    {
    case ComboBox::RANGE_TYPE_MIDDLE:
      m_smsData->m_range.min.y = m_smsData->m_target.y - rangeY / 2;
      m_smsData->m_range.max.y = m_smsData->m_target.y + rangeY / 2;
      break;
    case ComboBox::RANGE_TYPE_BOTTOM:
      m_smsData->m_range.min.y = m_smsData->m_target.y;
      m_smsData->m_range.max.y = m_smsData->m_target.y + rangeY;
      break;
    case ComboBox::RANGE_TYPE_TOP:
      m_smsData->m_range.min.y = m_smsData->m_target.y - rangeY;
      m_smsData->m_range.max.y = m_smsData->m_target.y;
      break;
    }

    if (m_smsData->m_range.min.y <= m_smsData->m_bounds.min.y)
    {
      m_smsData->m_range.min.y = m_smsData->m_bounds.min.y;
      m_lblRangeMinY->setStyleSheet("QLabel { color : red; }");
    }
    else
    {
      m_lblRangeMinY->setStyleSheet("QLabel { color : black; }");
    }
    if (m_smsData->m_range.max.y >= m_smsData->m_bounds.max.y)
    {
      m_smsData->m_range.max.y = m_smsData->m_bounds.max.y;
      m_lblRangeMaxY->setStyleSheet("QLabel { color : red; }");
    }
    else
    {
      m_lblRangeMaxY->setStyleSheet("QLabel { color : black; }");
    }
    m_lblRangeMinY->setText(QString::number(m_smsData->m_range.min.y));
    m_lblRangeMaxY->setText(QString::number(m_smsData->m_range.max.y));
  }
}

void FluffManipulator::updateRangeZ()
{
  float rangeZ = 0;

  std::stringstream ss(m_txbTargetZ->text().toStdString());
  ss >> std::dec;
  ss >> m_smsData->m_target.z;
  std::stringstream ss2(m_txbRangeZ->text().toStdString());
  ss2 >> std::dec;
  ss2 >> rangeZ;

  if (!ss.fail() && !ss2.fail())
  {
    switch (m_cmbRangeZType->currentIndex())
    {
    case ComboBox::RANGE_TYPE_MIDDLE:
      m_smsData->m_range.min.z = m_smsData->m_target.z - rangeZ / 2;
      m_smsData->m_range.max.z = m_smsData->m_target.z + rangeZ / 2;
      break;
    case ComboBox::RANGE_TYPE_BOTTOM:
      m_smsData->m_range.min.z = m_smsData->m_target.z;
      m_smsData->m_range.max.z = m_smsData->m_target.z + rangeZ;
      break;
    case ComboBox::RANGE_TYPE_TOP:
      m_smsData->m_range.min.z = m_smsData->m_target.z - rangeZ;
      m_smsData->m_range.max.z = m_smsData->m_target.z;
      break;
    default:
      break;
    }

    if (m_smsData->m_range.min.z <= m_smsData->m_bounds.min.z)
    {
      m_smsData->m_range.min.z = m_smsData->m_bounds.min.z;
      m_lblRangeMinZ->setStyleSheet("QLabel { color : red; }");
    }
    else
    {
      m_lblRangeMinZ->setStyleSheet("QLabel { color : black; }");
    }

    if (m_smsData->m_range.max.z >= m_smsData->m_bounds.max.z)
    {
      m_smsData->m_range.max.z = m_smsData->m_bounds.max.z;
      m_lblRangeMaxZ->setStyleSheet("QLabel { color : red; }");
    }
    else
    {
      m_lblRangeMaxZ->setStyleSheet("QLabel { color : black; }");
    }

    m_lblRangeMinZ->setText(QString::number(m_smsData->m_range.min.z));
    m_lblRangeMaxZ->setText(QString::number(m_smsData->m_range.max.z));
  }
}

void FluffManipulator::onRngSeedChanged()
{
  u32 rngSeed = 0;
  u32 rngIndex = 0;
  if (!m_txbRngSeed->text().isEmpty())
  {
    std::stringstream ss(m_txbRngSeed->text().toStdString());
    ss >> std::hex >> rngSeed;

    rngIndex = rng_functions::seed_to_index(rngSeed);
  }
  m_editedRngSeed = rngSeed;
  m_editedRngIndex = rngIndex;
  m_txbRngIndex->setText(QString::number(rngIndex));
}

void FluffManipulator::onRngIndexChanged()
{
  u32 rngSeed = 0;
  u32 rngIndex = 0;
  if (!m_txbRngIndex->text().isEmpty())
  {
    std::stringstream ss(m_txbRngIndex->text().toStdString());
    ss >> std::dec >> rngIndex;

    rngSeed = rng_functions::index_to_seed(rngIndex);
  }
  m_editedRngSeed = rngSeed;
  m_editedRngIndex = rngIndex;
  m_txbRngSeed->setText(QString::number(rngSeed, 16).toUpper());
}

void FluffManipulator::onSwingDirectionChanged()
{
  float swingDirection = 0;
  u32 hexSwingDirection = 0;
  if (!m_txbTargetSwingDirection->text().isEmpty())
  {
    std::stringstream ss(m_txbTargetSwingDirection->text().toStdString());
    ss >> swingDirection;
    std::memcpy(&hexSwingDirection, &swingDirection, sizeof(u32));
  }
  m_smsData->m_targetSwingDirection = swingDirection;
  m_txbTargetSwingDirectionHex->setText(QString::number(hexSwingDirection, 16).toUpper());
}

void FluffManipulator::onSwingDirectionHexChanged()
{
  float swingDirection = 0;
  u32 hexSwingDirection = 0;
  if (!m_txbTargetSwingDirectionHex->text().isEmpty())
  {
    std::stringstream ss(m_txbTargetSwingDirectionHex->text().toStdString());
    ss >> std::hex >> hexSwingDirection;
    std::memcpy(&swingDirection, &hexSwingDirection, sizeof(u32));
  }
  m_smsData->m_targetSwingDirection = swingDirection;
  m_txbTargetSwingDirection->setText(QString::fromStdString(std::to_string(swingDirection)));
}

void FluffManipulator::onSearchOriginCheckBoxChanged()
{
  float amplitudeX = 425; // 綿毛のX方向の最大振幅425
  float amplitudeZ = 425; // 綿毛のY方向の最大振幅425
  if (m_chbSearchOrigin->checkState() == Qt::Checked)
  {
    amplitudeX = 0;
    amplitudeZ = 0;
  }
  else
  {
    amplitudeX = 425;
    amplitudeZ = 425;
  }
  m_smsData->m_bounds.min.x = -5000 - amplitudeX;
  m_smsData->m_bounds.max.x = 5000 + amplitudeX;
  m_smsData->m_bounds.min.y = 0;
  m_smsData->m_bounds.max.y = 9139;
  m_smsData->m_bounds.min.z = -5000 - amplitudeZ;
  m_smsData->m_bounds.max.z = 5000 + amplitudeZ;
  m_txbTargetX->setValidator(
    new QIntValidator(m_smsData->m_bounds.min.x, m_smsData->m_bounds.max.x, this));
  m_txbTargetY->setValidator(
    new QIntValidator(m_smsData->m_bounds.min.y, m_smsData->m_bounds.max.y, this));
  m_txbTargetZ->setValidator(
    new QIntValidator(m_smsData->m_bounds.min.z, m_smsData->m_bounds.max.z, this));
  m_txbTargetX->setToolTip(QString::fromStdString(std::to_string(m_smsData->m_bounds.min.x) +
    " .. " +
    std::to_string(m_smsData->m_bounds.max.x)));
  m_txbTargetY->setToolTip(QString::fromStdString(std::to_string(m_smsData->m_bounds.min.y) +
    " .. " +
    std::to_string(m_smsData->m_bounds.max.y)));
  m_txbTargetZ->setToolTip(QString::fromStdString(std::to_string(m_smsData->m_bounds.min.z) +
    " .. " +
    std::to_string(m_smsData->m_bounds.max.z)));
}