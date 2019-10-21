#ifndef _PIPELINE_MANAGER_H
#define _PIPELINE_MANAGER_H

#include <string>
#include <vector>
#include <utility>
#include "camera_manager.h"
#include "qt/watch_qt.h"

namespace Ui {
class MainWindow;
}

class PipelineManager
{
public:
	PipelineManager(MainWindow *main_window);
	~PipelineManager();
	void add(std::string stream_address, CameraSettingsData &camera_settings_data);
	void remove(std::string &stream_address);
private:
	std::vector<Camera*> playlist;

	ObjectDetector *object_detector_face;
	ObjectDetector *object_detector_gender;
	ObjectDetector *object_detector_age;
	FaceRecognition *face_recognitor;

	MainWindow *m_main_window;
};

#endif /* _PIPELINE_MANAGER_H */
