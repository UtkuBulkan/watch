/*
 * Copyright 2019 SU Technology Ltd. All rights reserved.
 *
 * MIT License
 *
 * Copyright (c) 2019 SU Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#include <string>
#include <mutex>
#include <condition_variable>
#include <opencv2/opencv.hpp>

#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include <fstream>

#include "objectdetection.h"
#include "objecttracker.h"
#include "facerecognition.h"
#include "qt/watch_qt.h"

#include "qt/watch_qt_data.h"

#define PIPELINE_SIGNAL_START 0
#define PIPELINE_SIGNAL_STOP 1
#define PIPELINE_SIGNAL_CHANGE_SETTINGS 2

#define CATDETECTOR_SKIP_THIS_NUMBER_OF_FRAMES 24
//#define CATDETECTOR_ENABLE_OUTPUT_TO_VIDEO_FILE

class Camera
{
public:
	Camera(std::string input_device_name, CameraSettingsData &camera_settings_data);
	~Camera();
	void set_models(std::vector<ObjectDetector*> object_detectors, ObjectTracker *object_tracker, FaceRecognition *face_recognitor);
	void camera_set_ui(MainWindow *main_window);

	void loop();
	void display_statistics(cv::Mat &frame, std::string id, std::string gender, std::string age, cv::Point label_location);
private:
	std::string m_input_device_name;
	cv::VideoCapture capture;
	cv::VideoWriter outputVideo;

	std::vector<ObjectDetector*> m_object_detectors;
	ObjectTracker *m_object_tracker;
	FaceRecognition *m_face_recognitor;
	MainWindow *m_main_window;

	std::mutex mutex;
	std::condition_variable condition_variable;
};
