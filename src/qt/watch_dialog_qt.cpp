#include "watch_dialog_qt.h"
#include <syslog.h>

CameraSettingsWindow::CameraSettingsWindow(QString camera_label, CameraSettingsData &camera_settings_data)
{
	syslog(LOG_NOTICE, "CameraSettingsWindow::CameraSettingsWindow Start");
	checkbox_list.push_back(new QCheckBox("Face Detection"));
	checkbox_list.push_back(new QCheckBox("Face Recognition"));
	checkbox_list.push_back(new QCheckBox("Object Detection"));
	checkbox_list.push_back(new QCheckBox("People Counter"));
	checkbox_list.push_back(new QCheckBox("Gender Prediction"));
	checkbox_list.push_back(new QCheckBox("Age Prediction"));
	checkbox_list.push_back(new QCheckBox("Heat-Map Estimation"));
	checkbox_list.push_back(new QCheckBox("Record detections as output file"));
	apply_button  = new QPushButton("Apply");
	start_button = new QPushButton("Start");

	is_stream_active = camera_settings_data.face_detection;
	checkbox_list[CHECKBOX_FACE_DETECTION]->setChecked(camera_settings_data.face_detection > 0);
	checkbox_list[CHECKBOX_FACE_RECOGNITON]->setChecked(camera_settings_data.face_recognition > 0);
	checkbox_list[CHECKBOX_OBJECT_DETECTION]->setChecked(camera_settings_data.object_detection > 0);
	checkbox_list[CHECKBOX_PEOPLE_COUNTER]->setChecked(camera_settings_data.people_counter > 0);
	checkbox_list[CHECKBOX_GENDER_PREDICTION]->setChecked(camera_settings_data.gender_prediction > 0);
	checkbox_list[CHECKBOX_AGE_PREDICTION]->setChecked(camera_settings_data.age_prediction > 0);
	checkbox_list[CHECKBOX_HEAT_MAP_ESTIMATION]->setChecked(camera_settings_data.heat_map_estimation > 0);
	checkbox_list[CHECKBOX_RECORD_DETECTIONS_AS_OUTPUT_FILE]->setChecked(camera_settings_data.record_detections_as_output_file > 0);

	set_start_button_state(camera_settings_data.active);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(checkbox_list[CHECKBOX_FACE_DETECTION]);
	layout->addWidget(checkbox_list[CHECKBOX_FACE_RECOGNITON]);
	layout->addWidget(checkbox_list[CHECKBOX_OBJECT_DETECTION]);
	layout->addWidget(checkbox_list[CHECKBOX_PEOPLE_COUNTER]);
	layout->addWidget(checkbox_list[CHECKBOX_GENDER_PREDICTION]);
	layout->addWidget(checkbox_list[CHECKBOX_AGE_PREDICTION]);
	layout->addWidget(checkbox_list[CHECKBOX_HEAT_MAP_ESTIMATION]);
	layout->addWidget(checkbox_list[CHECKBOX_RECORD_DETECTIONS_AS_OUTPUT_FILE]);

	layout->addWidget(apply_button);
	layout->addWidget(start_button);

	this->setLayout(layout);
	this->setWindowTitle(camera_label);

	address = camera_label;

	connect(apply_button, &QPushButton::clicked,
			this, &CameraSettingsWindow::set_camera_specific_settings);

	connect(start_button, &QPushButton::clicked,
			this, &CameraSettingsWindow::start_camera_stream);

	syslog(LOG_NOTICE, "CameraSettingsWindow::CameraSettingsWindow End");
}

CameraSettingsWindow::~CameraSettingsWindow()
{

}

int CameraSettingsWindow::get_checkbox_state(int checkebox_index)
{
	return checkbox_list[checkebox_index]->checkState();
}

int CameraSettingsWindow::get_stream_active_state()
{
	return is_stream_active;
}

QString CameraSettingsWindow::get_dialog_current_address()
{
	return address;
}

void CameraSettingsWindow::set_start_button_state(int state)
{
	is_stream_active = state;
	if(state > 0) start_button->setText("Stop");
	else start_button->setText("Start");
}
