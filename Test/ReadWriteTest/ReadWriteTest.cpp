#include "ReadWriteTest.h"
#include "../../Common/CommonTypes.h"
#include <QTableWidgetItem>
#include <QTimer>

extern QTimer* g_timer_100ms;

ReadWriteTest::ReadWriteTest(QWidget* parent)
  : QWidget(parent) {
  ui.setupUi(this);

  items_.append(ReadWriteTestItem(Type::S8, 0x80000000));
  items_.append(ReadWriteTestItem(Type::U8, 0x80000000));
  items_.append(ReadWriteTestItem(Type::S16, 0x80000000));
  items_.append(ReadWriteTestItem(Type::U16, 0x80000000));
  items_.append(ReadWriteTestItem(Type::S32, 0x80000000));
  items_.append(ReadWriteTestItem(Type::U32, 0x80000000));
  items_.append(ReadWriteTestItem(Type::FLOAT, 0x812f96f4, "xz_speed dolpic49"));
  items_.append(ReadWriteTestItem(Type::STRING, 0x80000000, "Game ID"));
  items_.append(ReadWriteTestItem(Type::BYTE_ARRAY, 0x80000000, "array", 20));

  model_ = new ReadWriteTestModel(&items_);
  ui.table_read_write->setModel(model_);

  connect(g_timer_100ms, &QTimer::timeout, this, QOverload<>::of(&ReadWriteTest::refresh));
}

ReadWriteTest::~ReadWriteTest() {
}

void ReadWriteTest::refresh() {
  for (s64 i = 0; i < items_.size(); i++) {
    items_[i].read_memory();
  }
  model_->update_list();
}
