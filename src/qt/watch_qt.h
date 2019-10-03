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

protected:
	void closeEvent(QCloseEvent *event);
	private slots:
	void on_startBtn_pressed();

private:
	Ui::MainWindow *ui;
	QGraphicsPixmapItem pixmap;
	cv::VideoCapture video;

	void camera_pipeline_process();
};
#endif // MAINWINDOW_H
