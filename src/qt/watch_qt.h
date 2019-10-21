#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <syslog.h>
#include <string>

#include <QMainWindow>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QPixmap>
#include <QCloseEvent>
#include <QMessageBox>
#include <QApplication>
#include <QCoreApplication>
#include <QLayout>
#include <QListWidget>
#include <QScrollBar>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "database/watch_mysql.h"

#include "opencv2/opencv.hpp"
#include "watch_dialog_qt.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	void setPixmap(QImage &qimg);
	void add_detected_face(QImage &detected_face);
	void generate_camera_table();

protected:
	void closeEvent(QCloseEvent *event);
private slots:
	void on_startBtn_pressed();
	void on_push_button_for_settings_clicked();
	void on_push_button_for_start_camera_stream();

private:
	Ui::MainWindow *ui;
	CameraSettingsWindow *camera_settings_window;
	QGraphicsPixmapItem pixmap;
	cv::VideoCapture video;

	void add_camera_list_item(QString id, QString address, CameraSettingsData &camera_settings_data);
	void update_camera_list_item(QString address, CameraSettingsData &camera_settings_data);
	bool check_camera_list_item_exists(std::string address);
	void on_startBtn_pressed(std::string stream_address);
	bool check_stream_availability(std::string stream_address);
	void start_stream(std::string stream_address, CameraSettingsData &camera_settings_data);
	void stop_stream(std::string stream_address);
	void onTreeWidgetDoubleClicked(QTreeWidgetItem *item, int column);

	QListWidget *list_widget;

	DBConnection *dbconnection;
};
#endif // MAINWINDOW_H
