#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

private:
	Ui::MainWindow *ui;
	QGraphicsPixmapItem pixmap;
	cv::VideoCapture video;

	void add_camera_list_item(QString id, QString address, QString output_recording);
	bool check_camera_list_item_exists(std::string address);
	void camera_pipeline_process(std::string stream_address);
	void on_startBtn_pressed(std::string stream_address);
	void start_stream(std::string stream_address);
	void onTreeWidgetDoubleClicked(QTreeWidgetItem *item, int column);

	QListWidget *list_widget;

	DBConnection *dbconnection;
};
#endif // MAINWINDOW_H
