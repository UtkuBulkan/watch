#ifndef _PIPELINE_MANAGER_H
#define _PIPELINE_MANAGER_H

#include <string>
#include <vector>
#include <utility>
#include "camera_manager.h"

class PipelineManager
{
public:
	void add(Camera *camera);
	void remove(std::string &stream_address);
private:
	std::vector<Camera*> playlist;
};

#endif /* _PIPELINE_MANAGER_H */
