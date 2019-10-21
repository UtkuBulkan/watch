#include "pipeline_manager.h"

void PipelineManager::add(Camera *camera)
{
	syslog(LOG_NOTICE, "PipelineManager::add Start");
	playlist.push_back(camera);
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
