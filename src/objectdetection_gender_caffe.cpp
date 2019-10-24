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

#include "objectdetection_gender_caffe.h"

ObjectDetector_GenderCaffe::ObjectDetector_GenderCaffe()
{
	syslog (LOG_NOTICE, "ObjectDetector_GenderCaffe Constructor Begin");
	setup_model_for_detector("", "../data/gender_caffe/deploy_gender.prototxt", "../data/gender_caffe/gender_net.caffemodel");
	load_network_model_for_detector("Caffe");
	syslog (LOG_NOTICE, "ObjectDetector_GenderCaffe Constructor End");
}

ObjectDetector_GenderCaffe::~ObjectDetector_GenderCaffe() {
}

void ObjectDetector_GenderCaffe::draw_prediction_indicators(int classId, float confidence, int left, int top, int right, int bottom, cv::Mat& frame)
{
	(void)classId;
	(void)confidence;
	(void)left;
	(void)top;
	(void)right;
	(void)bottom;
	(void)frame;
	syslog(LOG_NOTICE, "ObjectDetector_GenderCaffe::draw_box Begin");
	syslog(LOG_NOTICE, "ObjectDetector_GenderCaffe::draw_box End");
}

void ObjectDetector_GenderCaffe::post_process(cv::Mat& frame, std::vector<cv::Mat> detection)
{
	(void)frame;
	(void)detection;
	syslog(LOG_NOTICE, "ObjectDetector_GenderCaffe::post_process Begin");
	syslog(LOG_NOTICE, "ObjectDetector_GenderCaffe::post_process End");
}

std::string ObjectDetector_GenderCaffe::process_frame(cv::Mat &frame, std::vector<std::pair<cv::Mat, cv::Point> > &detections) {
	(void)detections;
	syslog(LOG_NOTICE, "ObjectDetector_GenderCaffe::process_frame Begin");
	std::vector<std::string> genderList = {"Male", "Female"};

	cv::Mat blob;
	cv::dnn::blobFromImage(frame, blob, 1, cv::Size(227, 227), cv::Scalar(78.4263377603, 87.7689143744, 114.895847746), false);
	get_net().setInput(blob);
	// string gender_preds;
	std::vector<float> genderPreds = get_net().forward();
	// printing gender here
	// find max element index
	// distance function does the argmax() work in C++
	int max_index_gender = std::distance(genderPreds.begin(), max_element(genderPreds.begin(), genderPreds.end()));
	std::string gender = genderList[max_index_gender];
	syslog(LOG_NOTICE, "%s, max index : %d", gender.c_str(), max_index_gender);
	syslog(LOG_NOTICE, "ObjectDetector_GenderCaffe::process_frame End");

	return gender;
}
