#pragma once

#include <QAbstractTableModel>
#include <QJsonObject>
#include <QJsonArray>

class EditParametersModel : public QAbstractTableModel {
Q_OBJECT

public:
  EditParametersModel(QJsonObject& json, QObject* parent);
  ~EditParametersModel() override;

  enum {
    COLUMN_IS_LOCKED = 0,
    COLUMN_OFFSET,
    COLUMN_NAME,
    COLUMN_TYPE,
    COLUMN_SIZE,
    COLUMN_NOTES,
    COLUMN_COUNT
  };

  int columnCount(const QModelIndex& parent) const override;
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role) override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  void update_list();

private:
  QJsonObject json_class_;
  QJsonArray json_offsets_;
};
