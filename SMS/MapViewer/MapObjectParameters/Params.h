#pragma once
#include <QVariant>
#include <QColor>

#include "Common/CommonTypes.h"
#include "Memory/Memory.h"

using namespace memory;

class ParamsBase {
public:
  Q_DISABLE_COPY_MOVE(ParamsBase)
  explicit ParamsBase(u32 ptr);
  virtual ~ParamsBase() = default;

  QVariant data(s32 row) const;
  virtual QColor data_color() const;
  QVariant header_data(s32 row) const;
  s32 row() const;
  QString name() const;

protected:
  struct Param {
    QString name;
    std::function<QVariant()> calc;
  };

  u32 ptr_ = 0;
  QString name_;
  std::vector<Param> params_;
};

class ParamsMario : public ParamsBase {
public:
  explicit ParamsMario(u32 ptr);
};

class ParamsManagerBase : public ParamsBase {
public:
  explicit ParamsManagerBase(u32 ptr);
};

class ParamsObjBase : public ParamsBase {
public:
  explicit ParamsObjBase(u32 ptr);

  QColor data_color() const override;
};

class ParamsTNameKuri : public ParamsObjBase {
public:
  explicit ParamsTNameKuri(u32 ptr);
};

class ParamsTBossManta : public ParamsObjBase {
public:
  explicit ParamsTBossManta(u32 ptr);
};