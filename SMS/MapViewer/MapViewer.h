#pragma once

#include <QWidget>
#include "ui_MapViewer.h"

class MapViewer : public QWidget
{
    Q_OBJECT

public:
    MapViewer(QWidget *parent = Q_NULLPTR);
    ~MapViewer();

private:
    Ui::MapViewer ui;
};
