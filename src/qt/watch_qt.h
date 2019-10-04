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

protected:
	void closeEvent(QCloseEvent *event);
	private slots:
	void on_startBtn_pressed();

private:
	Ui::MainWindow *ui;
	QGraphicsPixmapItem pixmap;
	cv::VideoCapture video;

	void camera_pipeline_process();

	QListWidget *list_widget;
};
#endif // MAINWINDOW_H
