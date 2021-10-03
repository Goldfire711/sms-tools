#pragma once

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include "ui_MainWindow.h"
#include "SMS/Spin/Spin.h"
#include "SMS/ObjectViewer/ObjectViewer.h"
#include "SMS/MapViewer/MapViewer.h"
#include "SMS/MapViewer/MapPinnaBeach.h"
#include "SMS/FluffManipulator/FluffManipulator.h"
#include "SMS/FluffManipulator/MapPianta8.h"
#include "Test/TestMainWindow.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(QWidget* parent = Q_NULLPTR);
	void closeEvent(QCloseEvent* event) override;
	void hook_unhook_attempt();

	void show_widget_spin();
	void show_widget_map_viewer();
	void show_widget_object_viewer();
	void show_widget_fluff_manipulator();
	void on_update();

private:
	Ui::MainWindowClass ui;

	Spin* sms_spin_ = nullptr;
	ObjectViewer* sms_object_viewer_ = nullptr;
	MapViewer* sms_map_viewer_ = nullptr;
	SMSData* sms_data_ = nullptr;
	FluffManipulator* sms_fluff_manipulator_ = nullptr;

	TestMainWindow* test_main_window_ = nullptr;
};