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

#include "objectdetection_ssdcaffe.h"

ObjectDetector_SsdCaffe::ObjectDetector_SsdCaffe()
{
	syslog (LOG_NOTICE, "ObjectDetector_SsdCaffe Constructor Begin");
	setup_model_for_detector("", "../data/ssdcaffe/deploy.prototxt", "../data/ssdcaffe/res10_300x300_ssd_iter_140000_fp16.caffemodel");
	load_network_model_for_detector("Caffe");
	syslog (LOG_NOTICE, "ObjectDetector_SsdCaffe Constructor End");
}

ObjectDetector_SsdCaffe::~ObjectDetector_SsdCaffe() {
}

void ObjectDetector_SsdCaffe::draw_prediction_indicators(int classId, float confidence, int left, int top, int right, int bottom, cv::Mat& frame)
{
	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::draw_box Begin");
	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::draw_box End");
}

void ObjectDetector_SsdCaffe::post_process(cv::Mat& frame, std::vector<cv::Mat> detection)
{
	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::post_process Begin");
	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::post_process End");
}

std::vector<cv::Mat> ObjectDetector_SsdCaffe::process_frame(cv::Mat &frame) {
	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::process_frame Begin");

	const cv::Scalar meanVal(104.0, 177.0, 123.0);
	const double inScaleFactor = 1.0;
	int frameHeight = frame.rows;
	int frameWidth = frame.cols;
	cv::Mat inputBlob = cv::dnn::blobFromImage(frame, inScaleFactor, cv::Size(inWidth, inHeight), meanVal, false, false);

	get_net().setInput(inputBlob, "data");
	cv::Mat detection = get_net().forward("detection_out");

	cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

	std::vector<cv::Mat> detected_faces;

	for(int i = 0; i < detectionMat.rows; i++)
	{
		float confidence = detectionMat.at<float>(i, 2);

		if(confidence > get_confidence_threshold())
		{
			int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frameWidth);
			int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frameHeight);
			int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frameWidth);
			int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frameHeight);

			cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0),2, 4);
			//detected_faces.push_back(frame(cv::Rect(x1,y1,x2,y2)));
		}
	}

	double t = cv::getTickCount();
	double tt_opencvDNN = 0;
	double fpsOpencvDNN = 0;
	tt_opencvDNN = ((double)cv::getTickCount() - t)/cv::getTickFrequency();
	fpsOpencvDNN = 1/tt_opencvDNN;
	cv::putText(frame,
			cv::format("London South Bank University - Utku Bulkan - Frame processing time: %.2f ms ; FPS = %.2f", t / 1000, fpsOpencvDNN),
			cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.4, cv::Scalar(0, 0, 255), 4);

	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::process_frame End");

	return detected_faces;
}
