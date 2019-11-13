#include "pipeline_manager.h"

PipelineManager::PipelineManager(MainWindow *main_window) : m_main_window(main_window)
{
}

PipelineManager::~PipelineManager()
{

}

void PipelineManager::event_listener(int event_recieved)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_event_recieved = event_recieved;
	lock.unlock();
}

void PipelineManager::loop()
{
	syslog(LOG_NOTICE, "PipelineManager::run Start");
	event_listener(PIPELINE_SIGNAL_NO_EVENT);
	int loop_state = 1;

	while(loop_state)
	{
		for(int i = 0; i<(int)playlist.size(); i++) {
			playlist[i]->process_frame();
		}
		if(m_event_recieved != PIPELINE_SIGNAL_NO_EVENT) {
			std::unique_lock<std::mutex> lock(m_mutex);
			if(m_event_recieved == PIPELINE_SIGNAL_START) {
				loop_state = 1;
			} else if(m_event_recieved == PIPELINE_SIGNAL_STOP) {
				loop_state = 0;
			}
			m_event_recieved = PIPELINE_SIGNAL_NO_EVENT;
			lock.unlock();
		}
	}
	syslog(LOG_NOTICE, "PipelineManager::run End");
}

void PipelineManager::add(std::string stream_address, CameraSettingsData &camera_settings_data)
{
	syslog(LOG_NOTICE, "PipelineManager::add Start");
	for(int i = 0; i<(int)playlist.size(); i++) {
		if (playlist[i]->get_input_device_name() == stream_address) {
			syslog(LOG_NOTICE, "PipelineManager::add Already in pipeline, discarding request.");
			return;
		}
	}
	std::unique_lock<std::mutex> lock(m_mutex);
	//Camera camera("rtsp://ubnt:ubnt@192.168.1.118:554/s1");
	ObjectDetector *object_detector_face = ObjectDetector::GenerateDetector("SsdCaffe");
	ObjectDetector *object_detector_gender = ObjectDetector::GenerateDetector("GenderCaffe");
	ObjectDetector *object_detector_age = ObjectDetector::GenerateDetector("AgeCaffe");
	FaceRecognition *face_recognitor = new FaceRecognition();

	Camera *camera = new Camera(stream_address, camera_settings_data);
	camera->set_models({object_detector_face, object_detector_gender, object_detector_age}, NULL, face_recognitor);

	connect(camera, SIGNAL(loop_set_pixmap(QImage, QString)), m_main_window, SLOT(setPixmap(QImage, QString)));
	connect(camera, SIGNAL(loop_add_detected_face(QImage)), m_main_window, SLOT(add_detected_face(QImage)));

	if((int)playlist.size() == 0) {
		QThread *thread = new QThread;
		moveToThread(thread);
		QObject::connect(thread, SIGNAL(started()), this, SLOT(loop()));
		thread->start();
	}
	playlist.push_back(camera);

	lock.unlock();
	syslog(LOG_NOTICE, "PipelineManager::add End");
}

void PipelineManager::remove(std::string &stream_address)
{
	syslog(LOG_NOTICE, "PipelineManager::remove Start");
	std::unique_lock<std::mutex> lock(m_mutex);
	for(int i = 0; i<(int)playlist.size(); i++) {
		if (playlist[i]->get_input_device_name() == stream_address) {
			playlist.erase(playlist.begin() + i);
			if(playlist.size() == 0) event_listener(PIPELINE_SIGNAL_STOP);
			return;
		}
	}
	lock.unlock();
	syslog(LOG_NOTICE, "PipelineManager::remove End");
}
