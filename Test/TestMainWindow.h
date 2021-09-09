#pragma once

#include <QMainWindow>
#include "ui_TestMainWindow.h"

class TestMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    TestMainWindow(QWidget *parent = Q_NULLPTR);
    ~TestMainWindow();

private:
    Ui::TestMainWindow ui;
};
