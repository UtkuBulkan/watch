#include <QCoreApplication>
#include "watch_qt.h"
#include "ui_mainwindow.h"
#include "pipeline_manager.h"

PipelineManager *pipeline_manager;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->graphicsView->setScene(new QGraphicsScene(this));
	ui->graphicsView->scene()->addItem(&pixmap);

	ui->listWidget->setIconSize(QSize(100, 100));
	ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	dbconnection = new DBConnection("localhost", "utku", "utku");
	generate_camera_table();

	connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::onTreeWidgetDoubleClicked);

	pipeline_manager= new PipelineManager(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_push_button_for_settings_clicked()
{
	syslog(LOG_NOTICE, "MainWindow::on_push_button_for_start_camera_stream Start");
	std::cout << "Apply push button : " << camera_settings_window->get_checkbox_state(0) << std::endl;
	CameraSettingsData camera_settings_data;

	camera_settings_data.face_detection = camera_settings_window->get_checkbox_state(CHECKBOX_FACE_DETECTION);
	camera_settings_data.face_recognition = camera_settings_window->get_checkbox_state(CHECKBOX_FACE_RECOGNITON);
	camera_settings_data.object_detection = camera_settings_window->get_checkbox_state(CHECKBOX_OBJECT_DETECTION);
	camera_settings_data.people_counter = camera_settings_window->get_checkbox_state(CHECKBOX_PEOPLE_COUNTER);
	camera_settings_data.gender_prediction = camera_settings_window->get_checkbox_state(CHECKBOX_GENDER_PREDICTION);
	camera_settings_data.age_prediction = camera_settings_window->get_checkbox_state(CHECKBOX_AGE_PREDICTION);
	camera_settings_data.heat_map_estimation = camera_settings_window->get_checkbox_state(CHECKBOX_HEAT_MAP_ESTIMATION);
	camera_settings_data.record_detections_as_output_file = camera_settings_window->get_checkbox_state(CHECKBOX_RECORD_DETECTIONS_AS_OUTPUT_FILE);
	dbconnection->update_camera(camera_settings_window->get_dialog_current_address().toStdString(), camera_settings_data);
	update_camera_list_item(camera_settings_window->get_dialog_current_address(), camera_settings_data);
	syslog(LOG_NOTICE, "MainWindow::on_push_button_for_start_camera_stream End");
}

void MainWindow::on_push_button_for_start_camera_stream()
{
	syslog(LOG_NOTICE, "MainWindow::on_push_button_for_start_camera_stream Start");
	std::cout << "Apply push button : " << camera_settings_window->get_checkbox_state(0) << std::endl;
	CameraSettingsData camera_settings_data;

	if(camera_settings_window->get_stream_active_state() > 0) {
		camera_settings_data.active = false;
	} else {
		camera_settings_data.active = true;
	}
	camera_settings_window->set_start_button_state(camera_settings_data.active);
	camera_settings_data.face_detection = camera_settings_window->get_checkbox_state(CHECKBOX_FACE_DETECTION);
	camera_settings_data.face_recognition = camera_settings_window->get_checkbox_state(CHECKBOX_FACE_RECOGNITON);
	camera_settings_data.object_detection = camera_settings_window->get_checkbox_state(CHECKBOX_OBJECT_DETECTION);
	camera_settings_data.people_counter = camera_settings_window->get_checkbox_state(CHECKBOX_PEOPLE_COUNTER);
	camera_settings_data.gender_prediction = camera_settings_window->get_checkbox_state(CHECKBOX_GENDER_PREDICTION);
	camera_settings_data.age_prediction = camera_settings_window->get_checkbox_state(CHECKBOX_AGE_PREDICTION);
	camera_settings_data.heat_map_estimation = camera_settings_window->get_checkbox_state(CHECKBOX_HEAT_MAP_ESTIMATION);
	camera_settings_data.record_detections_as_output_file = camera_settings_window->get_checkbox_state(CHECKBOX_RECORD_DETECTIONS_AS_OUTPUT_FILE);
	dbconnection->update_camera(camera_settings_window->get_dialog_current_address().toStdString(), camera_settings_data);
	update_camera_list_item(camera_settings_window->get_dialog_current_address(), camera_settings_data);

	if(camera_settings_data.active > 0) {
		start_stream(camera_settings_window->get_dialog_current_address().toStdString(), camera_settings_data);
	} else {
		stop_stream(camera_settings_window->get_dialog_current_address().toStdString());
	}
	syslog(LOG_NOTICE, "MainWindow::on_push_button_for_start_camera_stream End");
}

void MainWindow::onTreeWidgetDoubleClicked(QTreeWidgetItem *item, int column)
{
	syslog(LOG_NOTICE, "MainWindow::onTreeWidgetDoubleClicked Start");
	std::cout << "Tree Widget Item double clicked:" << item->text(1).toStdString() << std::endl;

	CameraSettingsData camera_settings_data;
	camera_settings_data.active = item->text(2).toStdString() == "Yes" ? 1 : 0;
	camera_settings_data.face_detection = item->text(3).toStdString() == "Yes" ? 1 : 0;
	camera_settings_data.face_recognition = item->text(4).toStdString() == "Yes" ? 1 : 0;
	camera_settings_data.object_detection = item->text(5).toStdString() == "Yes" ? 1 : 0;
	camera_settings_data.people_counter = item->text(6).toStdString() == "Yes" ? 1 : 0;
	camera_settings_data.gender_prediction = item->text(7).toStdString() == "Yes" ? 1 : 0;
	camera_settings_data.age_prediction = item->text(8).toStdString() == "Yes" ? 1 : 0;
	camera_settings_data.heat_map_estimation = item->text(9).toStdString() == "Yes" ? 1 : 0;
	camera_settings_data.record_detections_as_output_file = item->text(10).toStdString() == "Yes" ? 1 : 0;

	camera_settings_window = new CameraSettingsWindow(item->text(1), camera_settings_data);
	connect(camera_settings_window,
			&CameraSettingsWindow::set_camera_specific_settings,
			this,
			&MainWindow::on_push_button_for_settings_clicked);
	connect(camera_settings_window,
			&CameraSettingsWindow::start_camera_stream,
			this,
			&MainWindow::on_push_button_for_start_camera_stream);
	camera_settings_window->exec();
	syslog(LOG_NOTICE, "MainWindow::onTreeWidgetDoubleClicked End");
}

void MainWindow::add_camera_list_item(QString id, QString address, CameraSettingsData &camera_settings_data)
{
	QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);

	treeItem->setText(0, id);
	treeItem->setText(1, address);
	treeItem->setText(2, camera_settings_data.active > 0 ? "Yes" : "No");
	treeItem->setText(3, camera_settings_data.face_detection > 0 ? "Yes" : "No");
	treeItem->setText(4, camera_settings_data.face_recognition > 0 ? "Yes" : "No");
	treeItem->setText(5, camera_settings_data.object_detection > 0 ? "Yes" : "No");
	treeItem->setText(6, camera_settings_data.people_counter > 0 ? "Yes" : "No");
	treeItem->setText(7, camera_settings_data.gender_prediction > 0 ? "Yes" : "No");
	treeItem->setText(8, camera_settings_data.age_prediction > 0 ? "Yes" : "No");
	treeItem->setText(9, camera_settings_data.heat_map_estimation > 0 ? "Yes" : "No");
	treeItem->setText(10, camera_settings_data.record_detections_as_output_file > 0 ? "Yes" : "No");
}

void MainWindow::update_camera_list_item(QString address, CameraSettingsData &camera_settings_data)
{
	for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
		QTreeWidgetItem *treeItem = ui->treeWidget->topLevelItem(i);
		QString current_address = treeItem->text(1);

		std::cout << "UPDATE Tree Widget Item double clicked:" << current_address.toStdString() << std::endl;

		if (current_address == address) {
			treeItem->setText(2, camera_settings_data.active > 0 ? "Yes" : "No");
			treeItem->setText(3, camera_settings_data.face_detection > 0 ? "Yes" : "No");
			treeItem->setText(4, camera_settings_data.face_recognition > 0 ? "Yes" : "No");
			treeItem->setText(5, camera_settings_data.object_detection > 0 ? "Yes" : "No");
			treeItem->setText(6, camera_settings_data.people_counter > 0 ? "Yes" : "No");
			treeItem->setText(7, camera_settings_data.gender_prediction > 0 ? "Yes" : "No");
			treeItem->setText(8, camera_settings_data.age_prediction > 0 ? "Yes" : "No");
			treeItem->setText(9, camera_settings_data.heat_map_estimation > 0 ? "Yes" : "No");
			treeItem->setText(10, camera_settings_data.record_detections_as_output_file > 0 ? "Yes" : "No");
			return;
		}
	}
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
	syslog(LOG_NOTICE, "MainWindow::generate_camera_table Start");
	std::vector<camera_list_item_t> camera_list;
	dbconnection->get_camera_list(camera_list);

	ui->treeWidget->setColumnCount(3);
	ui->treeWidget->setHeaderLabels(QStringList() << "Id" << "Address" << "active" << "face detection" << "face recognition" << "object detection"
			<< "people counter" << "gender prediction" << "age prediction" << "heat map" << "record as output");
	for(int i = 0; i < (int) camera_list.size(); i++) {
		CameraSettingsData camera_settings_data;
		camera_settings_data.active = camera_list[i].active;
		camera_settings_data.face_detection = camera_list[i].face_detection;
		camera_settings_data.face_recognition = camera_list[i].face_recognition;
		camera_settings_data.object_detection = camera_list[i].object_detection;
		camera_settings_data.people_counter = camera_list[i].people_counter;
		camera_settings_data.gender_prediction = camera_list[i].gender_prediction;
		camera_settings_data.age_prediction = camera_list[i].age_prediction;
		camera_settings_data.heat_map_estimation = camera_list[i].heat_map_estimation;
		camera_settings_data.record_detections_as_output_file = camera_list[i].record_detections_as_output_file;

		add_camera_list_item(QString::fromStdString(std::to_string(camera_list[i].id)),
				QString::fromStdString(camera_list[i].address),
				camera_settings_data);
	}
	syslog(LOG_NOTICE, "MainWindow::generate_camera_table End");
}

void MainWindow::on_startBtn_pressed()
{
	std::string stream_address = ui->videoEdit->text().trimmed().toStdString();

	if(check_stream_availability(stream_address) == false) {
		syslog(LOG_NOTICE, "Address for the Camera is not accessible.");
		return;
	}

	if(dbconnection->check_camera_exists(stream_address) == true) {
		syslog(LOG_NOTICE, "Camera has already been available in the database");
	} else {
		CameraSettingsData camera_settings_data;
		camera_settings_data.active = Qt::Unchecked;
		camera_settings_data.face_detection = Qt::Checked;
		camera_settings_data.face_recognition = Qt::Checked;
		camera_settings_data.object_detection = Qt::Unchecked;
		camera_settings_data.people_counter = Qt::Unchecked;
		camera_settings_data.gender_prediction = Qt::Checked;
		camera_settings_data.age_prediction = Qt::Checked;
		camera_settings_data.heat_map_estimation = Qt::Unchecked;
		camera_settings_data.record_detections_as_output_file = Qt::Unchecked;
		add_camera_list_item(QString("0"), QString::fromStdString(stream_address), camera_settings_data);
		dbconnection->add_camera(stream_address, camera_settings_data);
	}
}

bool MainWindow::check_stream_availability(std::string stream_address)
{
	cv::VideoCapture local_video;

	if(local_video.isOpened())
	{
		local_video.release();
		return false;
	}

	bool isCamera;
	int cameraIndex = ui->videoEdit->text().toInt(&isCamera);
	if(isCamera)
	{
		if(!local_video.open(cameraIndex))
		{
			QMessageBox::critical(this,
					"Camera Error",
					"Make sure you entered a correct camera index,"
					"<br>or that the camera is not being accessed by another program!");
			return false;
		}
	}
	else {
		if(!local_video.open(stream_address))
		{
			QMessageBox::critical(this,
					"Video Error",
					"Make sure you entered a correct and supported video file path,"
					"<br>or a correct RTSP feed URL!");
			return false;
		}
	}
	return true;
}

void MainWindow::start_stream(std::string stream_address, CameraSettingsData &camera_settings_data)
{
	syslog(LOG_NOTICE, "MainWindow::start_stream Start");

	pipeline_manager->add(stream_address, camera_settings_data);

	syslog(LOG_NOTICE, "MainWindow::start_stream End");
}

void MainWindow::stop_stream(std::string stream_address)
{
	syslog(LOG_NOTICE, "MainWindow::stop_stream Start");
	//camera_pipeline_stop(stream_address);
	pipeline_manager->remove(stream_address);
	syslog(LOG_NOTICE, "MainWindow::stop_stream End");
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
