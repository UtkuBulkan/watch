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
#include <string>
#include <syslog.h>

#include "objectdetection_yolo.h"
#include "objectdetection_yolotiny.h"
//#include "objectdetection_haar.h"
#include "objectdetection_ssdcaffe.h"
#include "objectdetection_gender_caffe.h"
#include "objectdetection_age_caffe.h"

ObjectDetector* ObjectDetector::GenerateDetector(std::string objectdetector_type_string)
{
	if (objectdetector_type_string == "Yolo")
		return new ObjectDetector_Yolo;
	else if (objectdetector_type_string == "Yolotiny")
		return new ObjectDetector_YoloTiny;
	else if (objectdetector_type_string == "SsdCaffe")
		return new ObjectDetector_SsdCaffe;
	else if (objectdetector_type_string == "GenderCaffe")
		return new ObjectDetector_GenderCaffe;
	else if (objectdetector_type_string == "AgeCaffe")
		return new ObjectDetector_AgeCaffe;

	return NULL;
}

ObjectDetector::ObjectDetector() : 	m_confidence_threshold(0.5), m_nms_threshold(0.4) {}

ObjectDetector::~ObjectDetector() {}

void ObjectDetector::setup_model_for_detector(std::string class_definition_file, std::string model_config_file, std::string model_weights_file)
{
	m_model_config_file = model_config_file;
	m_model_weights_file = model_weights_file;
	m_class_definition_file = class_definition_file;
}

void ObjectDetector::load_model_classes_for_detector()
{
	std::ifstream classes_file_stream(m_class_definition_file.c_str());
	std::string class_file_line;

	while (getline(classes_file_stream, class_file_line)) {
		classes.push_back(class_file_line);
		syslog (LOG_NOTICE, "Class Labels : %s", class_file_line.c_str());
	}

	std::ifstream colors_file_stream(colors_file.c_str());
	std::string colors_file_line;

	while (getline(colors_file_stream, colors_file_line)) {
		std::stringstream ss(colors_file_line);
		double red, green, blue;
		ss >> red >> green >> blue;
		colors.push_back(cv::Scalar(red, green, blue, 255.0));
		syslog (LOG_NOTICE, "Colors.txt Colors : %f, %f, %f", red, green, blue);
	}
}

void ObjectDetector::load_network_model_for_detector(std::string network_type)
{
	// Load the network for the model
	syslog (LOG_NOTICE, "ObjectDetector Loading Network");
	if (network_type == "Darknet") {
		net = cv::dnn::readNetFromDarknet(m_model_config_file, m_model_weights_file);
	} else if(network_type == "Caffe") {
		net = cv::dnn::readNetFromCaffe(m_model_config_file, m_model_weights_file);
	}
	net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
	syslog (LOG_NOTICE, "ObjectDetector Network Loaded");
}

std::vector < std::string > ObjectDetector::get_class_labels()
{
	return classes;
}

float ObjectDetector::get_confidence_threshold()
{
	return m_confidence_threshold;
}

float ObjectDetector::get_nms_threshold()
{
	return m_nms_threshold;
}

cv::dnn::Net ObjectDetector::get_net()
{
	return net;
}

#if 0
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <syslog.h>

#include "tensorflow/c/c_api.h"
#include "objectdetection.h"
#include "md5_helper.h"

#define CATDETECTOR_ANALYSE_EVERY_24_FRAMES
#define CATDETECTOR_ENABLE_OUTPUT_TO_VIDEO_FILE

#define inpHeight 1280
#define inpWidth 720

//#define DEEP_LEARNING_MODEL_YOLO_TINY
//#define DEEP_LEARNING_MODEL_YOLO
#define DEEP_LEARNING_MODEL_SSD_CAFFE
//#define DEEP_LEARNING_MODEL_SSD_TENSORFLOW

ObjectDetector::ObjectDetector() : get_confidence_threshold(0.5),
		get_nms_threshold(0.4),
	colors_file("../data/colors.txt"),
#ifdef DEEP_LEARNING_MODEL_YOLO
	class_definition_file("../data/yolo/coco.names"),
	model_config_file("../data/yolo/yolov3.cfg"),
	model_weights_file("../data/yolo/yolov3.weights")
#endif
#ifdef DEEP_LEARNING_MODEL_YOLO_TINY
	class_definition_file("../data/yolo/coco.names"),
	model_config_file("../data/yolo/yolov3.cfg"),
	model_weights_file("../data/yolo/yolov3.weights")
#endif
#ifdef DEEP_LEARNING_MODEL_SSD_CAFFE
	class_definition_file("../data/ssdcaffe/coco.names"),
	model_config_file("../data/ssdcaffe/deploy.prototxt"),
	model_weights_file("../data/ssdcaffe/res10_300x300_ssd_iter_140000_fp16.caffemodel")
#endif
#ifdef DEEP_LEARNING_MODEL_SSD_TENSORFLOW
	class_definition_file("../data/ssdtensorflow/coco.names"),
	model_config_file("../data/ssdtensorflow/opencv_face_detector.pbtxt"),
	model_weights_file("../data/ssdtensorflow/opencv_face_detector_uint8.pb")
#endif
{
	syslog (LOG_NOTICE, "ObjectDetector Constructor Begin");

	std::ifstream classes_file_stream(class_definition_file.c_str());
	std::string class_file_line;

	while (getline(classes_file_stream, class_file_line)) {
		classes.push_back(class_file_line);
		syslog (LOG_NOTICE, "Class Labels : %s", class_file_line.c_str());
	}

	std::ifstream colors_file_stream(colors_file.c_str());
	std::string colors_file_line;

	while (getline(colors_file_stream, colors_file_line)) {
		std::stringstream ss(colors_file_line);
		double red, green, blue;
		ss >> red >> green >> blue;
		colors.push_back(cv::Scalar(red, green, blue, 255.0));
		syslog (LOG_NOTICE, "Colors.txt Colors : %f, %f, %f", red, green, blue);
	}

	// Load the network for the model
	syslog (LOG_NOTICE, "ObjectDetector Loading Network");
#if defined DEEP_LEARNING_MODEL_YOLO && defined DEEP_LEARNING_MODEL_YOLO_TINY
	net = cv::dnn::readNetFromDarknet(model_config_file, model_weights_file);
#endif
#ifdef DEEP_LEARNING_MODEL_SSD_CAFFE
	net = cv::dnn::readNetFromCaffe(model_config_file, model_weights_file);
#endif
#ifdef DEEP_LEARNING_MODEL_SSD_TENSORFLOW
	net = cv::dnn::readNetFromTensorflow(model_config_file, model_weights_file);
#endif
	net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
	syslog (LOG_NOTICE, "ObjectDetector Network Loaded");

	multiTracker = cv::MultiTracker::create();

	syslog (LOG_NOTICE, "ObjectDetector Constructor End");
}

ObjectDetector::~ObjectDetector() {
}

void ObjectDetector::draw_prediction_indicators(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame)
{
	syslog(LOG_NOTICE, "ObjectDetector::draw_box Begin");
	//Draw a rectangle displaying the bounding box
	cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 0, 255));

	//Get the label for the class name and its confidence
	std::string label = cv::format("%.2f", conf);
	if (!classes.empty())
	{
		CV_Assert(classId < (int)classes.size());
		label = classes[classId] + ":" + label;
	}

	//Display the label at the top of the bounding box
	int baseLine;
	cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
	top = std::max(top, labelSize.height);
	cv::putText(frame, label, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255,255,255));

	syslog(LOG_NOTICE, "ObjectDetector::draw_box End");
}

void ObjectDetector::post_process(cv::Mat& frame, cv::Mat& detection)
{
	syslog(LOG_NOTICE, "ObjectDetector::post_process Begin");

	cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

	for(int i = 0; i < detectionMat.rows; i++)
	{
		float confidence = detectionMat.at<float>(i, 2);

		if(confidence > confidence_threshold)
		{
			int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.size().width);
			int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.size().height);
			int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.size().width);
			int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.size().height);

			cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0),2, 4);
		}
	}
#if 0
	// Perform non maximum suppression to eliminate redundant overlapping boxes with
	// lower confidences
	std::vector<int> indices;
	cv::dnn::NMSBoxes(boxes, confidences, confidence_threshold, nmsThreshold, indices);
	for (size_t i = 0; i < indices.size(); ++i)
	{
		int idx = indices[i];
		cv::Rect box = boxes[idx];
		drawPred(classIds[idx], confidences[idx], box.x, box.y,
				box.x + box.width, box.y + box.height, frame);
	}
#endif
	syslog(LOG_NOTICE, "ObjectDetector::post_process End");
}

// Get the names of the output layers
std::vector<std::string> ObjectDetector::getOutputsNames()
{
	static std::vector<std::string> names;
	if (names.empty())
	{
		//Get the indices of the output layers, i.e. the layers with unconnected outputs
		std::vector<int> outLayers = net.getUnconnectedOutLayers();

		//get the names of all the layers in the network
		std::vector<std::string> layersNames = net.getLayerNames();

		// Get the names of the output layers in names
		names.resize(outLayers.size());
		for (size_t i = 0; i < outLayers.size(); ++i)
			names[i] = layersNames[outLayers[i] - 1];
	}
	return names;
}

void ObjectDetector::process_frame(cv::Mat &frame) {
	std::vector<std::string> outNames(2);
	std::vector<double> layersTimes;
	std::string label;
	cv::Mat blob;
	double freq, t;

	syslog(LOG_NOTICE, "ObjectDetector::process_frame Begin");
#if 0
	//cv::dnn::blobFromImage(frame, blob);
	cv::dnn::blobFromImage(frame, blob, 1.0, cv::Size(inpWidth, inpHeight), cv::Scalar(104.0, 177.0, 123.0), true, false);
#endif
	cv::dnn::blobFromImage(frame, blob, 1.0, cv::Size(inpWidth, inpHeight), cv::Scalar(104.0, 177.0, 123.0), false, false);
	//cv::Mat inputBlob = cv::dnn::blobFromImage(frameOpenCVDNN, inScaleFactor, cv::Size(inWidth, inHeight), meanVal, true, false);

	net.setInput(blob);
	cv::Mat detection;
	net.forward(detection);

#if 0
	std::vector<cv::Mat> detections;
	net.forward(detections, getOutputsNames());
#endif
	//syslog(LOG_NOTICE, "Number of detections : %d", (int) detections.size());

	//object_tracker_with_new_frame(frame, detections);

	post_process(frame, detection);
	freq = cv::getTickFrequency() / 1000;
	t = net.getPerfProfile(layersTimes) / freq;
	label = cv::format("London South Bank University - Utku Bulkan - Frame processing time: %.2f ms", t);
	cv::putText(frame, label, cv::Point(0, 20), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0));


	syslog(LOG_NOTICE, "ObjectDetector::process_frame End");
}

void ObjectDetector::loop() {
	cv::Mat frame;
	cv::VideoCapture capture;
	cv::VideoWriter outputVideo;

	syslog(LOG_NOTICE, "Opening file : %s", filename.c_str());

	capture.open(filename);
	if ( !capture.isOpened	() ) {
		throw "Error opening file.\n";
	}

	capture >> frame;

	int ex = static_cast<int>(capture.get(cv::CAP_PROP_FOURCC));	// Get Codec Type- Int form
	int codec = cv::VideoWriter::fourcc('M', 'P', 'G', '2');
	cv::Size S = cv::Size((int) capture.get(cv::CAP_PROP_FRAME_WIDTH), (int) capture.get(cv::CAP_PROP_FRAME_HEIGHT));

	syslog(LOG_NOTICE, "Input file fourcc: %d, %d", codec, ex);
	syslog(LOG_NOTICE, "Input file width: %d", S.width);
	syslog(LOG_NOTICE, "Input file height: %d", S.height);

	outputVideo.open("./output.mp4", cv::CAP_FFMPEG, codec, capture.get(cv::CAP_PROP_FPS), S, true);
	outputVideo << frame;

	cv::namedWindow("Camera1", cv::WINDOW_NORMAL);
	cv::resizeWindow("Camera1", 640, 480);

	int framecount = 0;

	while(1) {
		syslog(LOG_NOTICE, "Frame count : %d", framecount);
		syslog(LOG_NOTICE, "Frame resolution : %d x %d", frame.rows, frame.cols);

		capture >> frame;
		if (frame.empty()) {
			syslog(LOG_NOTICE, "Last read frame is empty, quitting.");
			break;
		}

		framecount++;
		{
#ifdef CATDETECTOR_ANALYSE_EVERY_24_FRAMES
			if (framecount % 24 == 0)
#endif
			process_frame(frame);
			//object_tracker_update_only(frame);
#ifdef CATDETECTOR_ENABLE_OUTPUT_TO_VIDEO_FILE
			/* Outputting captured frames to a video file */
			outputVideo << frame;
#endif
			cv::imshow("Camera1", frame);

			/* Sending the data as a Kafka producer */
			/* video_analyser_kafka_producer(j.dump().c_str(), "TutorialTopic"); */
		}
		if(cv::waitKey(30) >= 0) break;
	}
	outputVideo.release();
}
#endif
