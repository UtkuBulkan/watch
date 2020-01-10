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
#include <sys/time.h>

#include "objectdetection_ssdcaffe.h"

#define inHeight 300
#define inWidth 300

ObjectDetector_SsdCaffe::ObjectDetector_SsdCaffe()
{
	syslog (LOG_NOTICE, "ObjectDetector_SsdCaffe Constructor Begin");

	syslog (LOG_NOTICE, "ObjectDetector_SsdCaffe Constructor End");
}

ObjectDetector_SsdCaffe::~ObjectDetector_SsdCaffe() {
}

void ObjectDetector_SsdCaffe::draw_prediction_indicators(int classId, float confidence, int left, int top, int right, int bottom, cv::Mat &output_frame)
{
	(void)classId;
	(void)confidence;
	(void)left;
	(void)top;
	(void)right;
	(void)bottom;
	(void)output_frame;
	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::draw_box Begin");
	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::draw_box End");
}

void ObjectDetector_SsdCaffe::post_process(cv::Mat& frame, cv::Mat &output_frame, std::vector<cv::Mat> detection)
{
	(void)frame;
	(void)output_frame;
	(void)detection;
	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::post_process Begin");
	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::post_process End");
}

long long current_timestamp() {
	struct timeval te;
	gettimeofday(&te, NULL);
	long long  miliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
	return miliseconds;
}

std::string ObjectDetector_SsdCaffe::process_frame(cv::Mat &frame, cv::Mat &output_frame, std::vector<std::pair<cv::Mat, cv::Point> > &detections) {
	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::process_frame Begin");

    //long long time_start = current_timestamp();
    //The GroupThreshold and ScaleFactor are the two important parameters 
    //decrease will get more hits, with more false positives
    int _hitThreshold = 0;// //going mad tuning this for cuda// not to be adjusted
    double _scaleFactor = 1.01;//1.05;// 20 sec --> huge impact on performance

    cv::cvtColor(img, frame, COLOR_BGR2BGRA);// COLOR_BGR2BGRA);
    GpuMat gpuFrame(frame);
    gpu_hog->setScaleFactor(_scaleFactor);
    gpu_hog->setNumLevels(hogLevels); 
    gpu_hog->setWinStride(win_stride);
    //gpu_hog->setHitThreshold(0); // play with this at your own risk :)
    gpu_hog->setGroupThreshold(hogGroupThreshold);// setting it to higher will reduce false positives// give all
    gpu_hog->detectMultiScale(gpuFrame, found);
    drawMarker(img, found, Scalar(255, 0, 0), 1);//BGR
    gpu_hog->setGroupThreshold(hogGroupThreshold*3);// setting it to higher will group more
    gpu_hog->detectMultiScale(gpuFrame, found);
    drawMarker(img, found, Scalar(0, 255, 0));//BGR
    syslog(LOG_NOTICE, "%d - %d,%d,%d,%d, framewidth : %d, frameheight : %d", i, x1, y1, x2-x1, y2-y1, frameWidth, frameHeight);

    cv::Mat face_detected;
    gpuFrame(cv::Rect(x1,y1,x2-x1,y2-y1)).copyTo(face_detected);
    detections.push_back(std::make_pair(face_detected, cv::Point(x2+3,y1+22)));

	//long long time_end= current_timestamp();

	//double fps = 1.0/(time_end - time_start);
	//cv::putText(frame,
	//		cv::format("LSBU - Utku Bulkan - freame = %.2lf", fps),
	//		cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.4, cv::Scalar(0, 0, 255), 4);

	syslog(LOG_NOTICE, "ObjectDetector_SsdCaffe::process_frame End");

	return std::string("");
}
