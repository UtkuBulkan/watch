#ifndef WATCH_QT_DATA_HEADER
#define WATCH_QT_DATA_HEADER

class CameraSettingsData
{
public:
	int active;
	int face_detection;
	int face_recognition;
	int object_detection;
	int people_counter;
	int gender_prediction;
	int age_prediction;
	int heat_map_estimation;
	int record_detections_as_output_file;
    int loop_video;
	int skip_frames;
};

#endif
