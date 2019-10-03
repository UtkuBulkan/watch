#include "watch_qt.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->graphicsView->setScene(new QGraphicsScene(this));
	ui->graphicsView->scene()->addItem(&pixmap);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_startBtn_pressed()
{
	using namespace cv;

	if(video.isOpened())
	{
		ui->startBtn->setText("Start");
		video.release();
		return;
	}

	bool isCamera;
	int cameraIndex = ui->videoEdit->text().toInt(&isCamera);
	if(isCamera)
	{
		if(!video.open(cameraIndex))
		{
			QMessageBox::critical(this,
					"Camera Error",
					"Make sure you entered a correct camera index,"
					"<br>or that the camera is not being accessed by another program!");
			return;
		}
	}
	else {
		if(!video.open(ui->videoEdit->text().trimmed().toStdString()))
		{
			QMessageBox::critical(this,
					"Video Error",
					"Make sure you entered a correct and supported video file path,"
					"<br>or a correct RTSP feed URL!");
			return;
		}
	}

	ui->startBtn->setText("Stop");

	Mat frame;
	while(video.isOpened())
	{
		video >> frame;
		if(!frame.empty())
		{
			QImage qimg(frame.data,
					frame.cols,
					frame.rows,
					frame.step,
					QImage::Format_RGB888);
			pixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()) );
			ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);
		}
		qApp->processEvents();
	}

	ui->startBtn->setText("Start");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if(video.isOpened())
	{
		QMessageBox::warning(this,
				"Warning",
				"Stop the video before closing the application!");
		event->ignore();
	}
	else
	{
		event->accept();
	}
}
