/*
 * Copyright 2018 SU Technology Ltd. All rights reserved.
 *
 * MIT License
 *
 * Copyright (c) 2018 SU Technology
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
 * Some parts of this file has been inspired and influenced from this link :
 * https://www.learnopencv.com/deep-learning-based-object-detection
 * -and-instance-segmentation-using-mask-r-cnn-in-opencv-python-c/
 *
 */
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <syslog.h>

#include "objectdetection_age_caffe.h"

ObjectDetector_AgeCaffe::ObjectDetector_AgeCaffe()
{
	syslog (LOG_NOTICE, "ObjectDetector_AgeCaffe Constructor Begin");
	setup_model_for_detector("", "../data/age_caffe/deploy_age.prototxt", "../data/age_caffe/age_net.caffemodel");
	load_network_model_for_detector("Caffe");
	syslog (LOG_NOTICE, "ObjectDetector_AgeCaffe Constructor End");
}

ObjectDetector_AgeCaffe::~ObjectDetector_AgeCaffe() {
}

void ObjectDetector_AgeCaffe::draw_prediction_indicators(int classId, float confidence, int left, int top, int right, int bottom, cv::Mat& frame)
{
	(void)classId;
	(void)confidence;
	(void)left;
	(void)top;
	(void)right;
	(void)bottom;
	(void)frame;
	syslog(LOG_NOTICE, "ObjectDetector_AgeCaffe::draw_box Begin");
	syslog(LOG_NOTICE, "ObjectDetector_AgeCaffe::draw_box End");
}

void ObjectDetector_AgeCaffe::post_process(cv::Mat& frame, std::vector<cv::Mat> detection)
{
	(void)frame;
	(void)detection;
	syslog(LOG_NOTICE, "ObjectDetector_AgeCaffe::post_process Begin");
	syslog(LOG_NOTICE, "ObjectDetector_AgeCaffe::post_process End");
}

std::string ObjectDetector_AgeCaffe::process_frame(cv::Mat &frame, std::vector<std::pair<cv::Mat, cv::Point> > &detections) {
	(void)detections;
	syslog(LOG_NOTICE, "ObjectDetector_AgeCaffe::process_frame Begin");
	std::vector<std::string> ageList = {"(0-2)", "(4-6)", "(8-12)", "(15-20)", "(25-32)", "(38-43)", "(48-53)", "(60-100)"};

	cv::Mat blob;
	cv::dnn::blobFromImage(frame, blob, 1, cv::Size(227, 227), cv::Scalar(78.4263377603, 87.7689143744, 114.895847746), false);
	get_net().setInput(blob);
	std::vector<float> agePreds = get_net().forward();
	int max_indice_age = std::distance(agePreds.begin(), std::max_element(agePreds.begin(), agePreds.end()));
	std::string age = ageList[max_indice_age];
	syslog(LOG_NOTICE, "%s, max index : %d", age.c_str(), max_indice_age);
	syslog(LOG_NOTICE, "ObjectDetector_AgeCaffe::process_frame End");

	return age;
}
