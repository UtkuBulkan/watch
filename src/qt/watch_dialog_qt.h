#ifndef CAMERASETTINGSWINDOW_H
#define CAMERASETTINGSWINDOW_H

#include <vector>
#include <QDialog>
#include <QString>
#include <QSignalMapper>
#include <QCheckBox>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>

#include "watch_qt_data.h"

#define CHECKBOX_FACE_DETECTION 0
#define CHECKBOX_FACE_RECOGNITON 1
#define CHECKBOX_OBJECT_DETECTION 2
#define CHECKBOX_PEOPLE_COUNTER 3
#define CHECKBOX_GENDER_PREDICTION 4
#define CHECKBOX_AGE_PREDICTION 5
#define CHECKBOX_HEAT_MAP_ESTIMATION 6
#define CHECKBOX_RECORD_DETECTIONS_AS_OUTPUT_FILE 7
#define CHECKBOX_LOOP_VIDEO 8
#define CHECKBOX_SKIP_FRAMES 9

#define STREAM_ACTIVE_STATE 10

class CameraSettingsWindow : public QDialog
{
	Q_OBJECT

public:
	CameraSettingsWindow(QString camera_label, CameraSettingsData &camera_settings_data);
	~CameraSettingsWindow();
	int get_checkbox_state(int checkebox_index);
	int get_stream_active_state();
	QString get_dialog_current_address();

	void set_start_button_state(int state);
signals:
	void set_camera_specific_settings();
	void start_camera_stream();
private:
	std::vector<QCheckBox*> checkbox_list;
	QString address;
	QSignalMapper *mapper;
	QPushButton *apply_button;
	QPushButton *start_button;

	int is_stream_active;
};
#endif /*CAMERASETTINGSWINDOW_H*/
