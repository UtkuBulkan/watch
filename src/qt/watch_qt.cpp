#include "watch_qt.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>

#include <syslog.h>
#include "camera_manager.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->graphicsView->setScene(new QGraphicsScene(this));
	ui->graphicsView->scene()->addItem(&pixmap);

	ui->listWidget->setIconSize(QSize(100, 100));

	dbconnection = new DBConnection("localhost", "utku", "utku");
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::add_camera_list_item(QString id, QString address, QString output_recording)
{

	QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);

	treeItem->setText(0, id);
	treeItem->setText(1, address);
	treeItem->setText(2, output_recording);
}

bool MainWindow::check_camera_list_item_exists(std::string address)
{
	for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
		QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
		QString current_address = item->text(1);
		if (current_address == QString::fromStdString(address)) {
			return true;
		}
	}
	return false;
}

void MainWindow::generate_camera_table()
{
	std::vector<camera_list_item_t> camera_list;
	dbconnection->get_camera_list(camera_list);

	ui->treeWidget->setColumnCount(3);
	ui->treeWidget->setHeaderLabels(QStringList() << "Id" << "Address" << "Record_output");
	for(int i = 0; i < (int) camera_list.size(); i++) {
		if(check_camera_list_item_exists(camera_list[i].address) == false)
		add_camera_list_item(QString::fromStdString(std::to_string(camera_list[i].id)),
				QString::fromStdString(camera_list[i].address),
				QString::fromStdString(std::to_string(camera_list[i].is_record_as_output)));
	}
}

void MainWindow::on_startBtn_pressed()
{
	using namespace cv;

	dbconnection->add_camera(ui->videoEdit->text().trimmed().toStdString());
	generate_camera_table();

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

	camera_pipeline_process();

	ui->startBtn->setText("Start");
}

void MainWindow::camera_pipeline_process()
{
	setlogmask (LOG_UPTO (LOG_DEBUG));
	openlog ("watch", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	ObjectDetector *object_detector_face = ObjectDetector::GenerateDetector("SsdCaffe");
	ObjectDetector *object_detector_gender = ObjectDetector::GenerateDetector("GenderCaffe");
	ObjectDetector *object_detector_age = ObjectDetector::GenerateDetector("AgeCaffe");
	FaceRecognition *face_recognitor = new FaceRecognition();
	//ObjectTracker *object_tracker = new ObjectTracker("KCF");
	//Camera camera("rtsp://ubnt:ubnt@192.168.1.118:554/s1");
	Camera camera(ui->videoEdit->text().trimmed().toStdString());
	camera.loop({object_detector_face, object_detector_gender, object_detector_age}, NULL, face_recognitor, this);

	closelog ();
}

void MainWindow::setPixmap(QImage &qimg)
{
	pixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()) );
	ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);
	qApp->processEvents();
}

void MainWindow::add_detected_face(QImage &detected_face)
{
	int is_scroll_to_bottom = false;
	QScrollBar *vertical_scroll_bar = ui->listWidget->verticalScrollBar();
	if(vertical_scroll_bar->value() == vertical_scroll_bar->maximum()) is_scroll_to_bottom = true;

	QListWidgetItem *item = new QListWidgetItem;
	item->setIcon(QPixmap::fromImage(detected_face.rgbSwapped()).scaled(80,80, Qt::KeepAspectRatio));
	ui->listWidget->insertItem(ui->listWidget->count(),item);
	if(is_scroll_to_bottom) ui->listWidget->scrollToBottom();
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
