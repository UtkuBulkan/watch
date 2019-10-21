#include "pipeline_manager.h"

PipelineManager::PipelineManager(MainWindow *main_window) : m_main_window(main_window)
{
	object_detector_face = ObjectDetector::GenerateDetector("SsdCaffe");
	object_detector_gender = ObjectDetector::GenerateDetector("GenderCaffe");
	object_detector_age = ObjectDetector::GenerateDetector("AgeCaffe");
	face_recognitor = new FaceRecognition();
	//ObjectTracker *object_tracker = new ObjectTracker("KCF");
}

PipelineManager::~PipelineManager()
{

}

void PipelineManager::add(std::string stream_address, CameraSettingsData &camera_settings_data)
{
	syslog(LOG_NOTICE, "PipelineManager::add Start");
	//Camera camera("rtsp://ubnt:ubnt@192.168.1.118:554/s1");
	Camera camera(stream_address, camera_settings_data);
	camera.set_models({object_detector_face, object_detector_gender, object_detector_age}, NULL, face_recognitor);
	camera.camera_set_ui(m_main_window);
	playlist.push_back(&camera);

	camera.start_thread();
	syslog(LOG_NOTICE, "PipelineManager::add End");
}

void PipelineManager::remove(std::string &stream_address)
{
	syslog(LOG_NOTICE, "PipelineManager::remove Start");
	for(int i = 0; i<(int)playlist.size(); i++) {
		if (playlist[i]->get_input_device_name() == stream_address) {
			playlist[i]->event_listener(PIPELINE_SIGNAL_STOP);
			return;
		}
	}
	syslog(LOG_NOTICE, "PipelineManager::remove End");
}
