#ifndef _PIPELINE_MANAGER_H
#define _PIPELINE_MANAGER_H

#include <string>
#include <vector>
#include <utility>
#include "camera_manager.h"
#include "qt/watch_qt.h"

#include <QObject>
#include <QThread>

namespace Ui {
class MainWindow;
}

#define PIPELINE_SIGNAL_NO_EVENT 0
#define PIPELINE_SIGNAL_START 2
#define PIPELINE_SIGNAL_STOP 4
#define PIPELINE_SIGNAL_CHANGE_SETTINGS 8

class PipelineManager : public QObject
{
	Q_OBJECT
public:
	PipelineManager(MainWindow *main_window);
	~PipelineManager();
	void add(std::string stream_address, CameraSettingsData &camera_settings_data);
	void remove(std::string &stream_address);
private:
	void event_listener(int event_recieved);
private slots:
	void loop();
private:
	std::vector<Camera*> playlist;

	MainWindow *m_main_window;

	std::mutex m_mutex;
	int m_event_recieved;
	
	FaceRecognition *face_recognitor;
};

#endif /* _PIPELINE_MANAGER_H */
