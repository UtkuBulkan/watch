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

#include "objectdetection_yolotiny.h"

ObjectDetector_YoloTiny::ObjectDetector_YoloTiny()
{
	syslog (LOG_DEBUG, "ObjectDetector_YoloTiny Constructor Begin");
	setup_model_for_detector("../data/yolo/coco.names", "../data/yolotiny/yolov3-tiny.cfg", "../data/yolotiny/yolov3-tiny.weights");
	load_model_classes_for_detector();
	load_network_model_for_detector("Darknet");
	syslog (LOG_DEBUG, "ObjectDetector_YoloTiny Constructor End");
}

ObjectDetector_YoloTiny::~ObjectDetector_YoloTiny() {
}

std::vector<std::string> ObjectDetector_YoloTiny::get_output_layer_names()
{
	// Get the names of the output layers
	static std::vector<std::string> names;
	if (names.empty())
	{
		//Get the indices of the output layers, i.e. the layers with unconnected outputs
		std::vector<int> outLayers = get_net().getUnconnectedOutLayers();

		//get the names of all the layers in the network
		std::vector<std::string> layersNames = get_net().getLayerNames();

		// Get the names of the output layers in names
		names.resize(outLayers.size());
		for (size_t i = 0; i < outLayers.size(); ++i)
			names[i] = layersNames[outLayers[i] - 1];
	}
	return names;
}

void ObjectDetector_YoloTiny::draw_prediction_indicators(int classId, float confidence, int left, int top, int right, int bottom, cv::Mat& frame)
{
	syslog(LOG_DEBUG, "ObjectDetector_YoloTiny::draw_box Begin");

	cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 0, 255));
	std::string confidence_and_class_information = cv::format("%.2f", confidence);
	if (!get_class_labels().empty())
	{
		CV_Assert(classId < (int)get_class_labels().size());
		confidence_and_class_information = get_class_labels()[classId] + ":" + confidence_and_class_information;
	}

	//Display the label at the top of the bounding box
	int baseLine;
	cv::Size labelSize = cv::getTextSize(confidence_and_class_information, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
	top = std::max(top, labelSize.height);
	cv::putText(frame, confidence_and_class_information, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255,255,255));

	syslog(LOG_DEBUG, "ObjectDetector_YoloTiny::draw_box End");
}

void ObjectDetector_YoloTiny::post_process(cv::Mat& frame, std::vector<cv::Mat> detection)
{
	syslog(LOG_DEBUG, "ObjectDetector_YoloTiny::post_process Begin");

	std::vector<int> classIds;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;

	for (size_t i = 0; i < detection.size(); ++i)
	{
		// Scan through all the bounding boxes output from the network and keep only the
		// ones with high confidence scores. Assign the box's class label as the class
		// with the highest score for the box.
		float* data = (float*)detection[i].data;
		for (int j = 0; j < detection[i].rows; ++j, data += detection[i].cols)
		{
			cv::Mat scores = detection[i].row(j).colRange(5, detection[i].cols);
			cv::Point classIdPoint;
			double confidence;
			// Get the value and location of the maximum score
			cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
			if (confidence > get_confidence_threshold())
			{
				int centerX = (int)(data[0] * frame.cols);
				int centerY = (int)(data[1] * frame.rows);
				int width = (int)(data[2] * frame.cols);
				int height = (int)(data[3] * frame.rows);
				int left = centerX - width / 2;
				int top = centerY - height / 2;

				classIds.push_back(classIdPoint.x);
				confidences.push_back((float)confidence);
				boxes.push_back(cv::Rect(left, top, width, height));
			}
		}
	}

	// Perform non maximum suppression to eliminate redundant overlapping boxes with lower confidences
	std::vector<int> indices;
	cv::dnn::NMSBoxes(boxes, confidences, get_confidence_threshold(), get_nms_threshold(), indices);
	for (size_t i = 0; i < indices.size(); ++i)
	{
		int idx = indices[i];
		cv::Rect box = boxes[idx];
		draw_prediction_indicators(classIds[idx], confidences[idx], box.x, box.y, box.x + box.width, box.y + box.height, frame);
	}
	syslog(LOG_DEBUG, "ObjectDetector_YoloTiny::post_process End");
}

std::string ObjectDetector_YoloTiny::process_frame(cv::Mat &frame, std::vector<std::pair<cv::Mat, cv::Point> > &detections)
{
	std::vector<std::string> outNames(2);
	std::vector<double> layersTimes;
	std::string label;
	cv::Mat blob;
	double freq, t;
	(void)detections;

	syslog(LOG_DEBUG, "ObjectDetector_YoloTiny::process_frame Begin");

	cv::dnn::blobFromImage(frame, blob, 1/255.0, cv::Size(inpWidth, inpHeight), cv::Scalar(0, 0, 0), true, false);
	get_net().setInput(blob);
	std::vector<cv::Mat> detection;
	get_net().forward(detection, get_output_layer_names());
	syslog(LOG_NOTICE, "Number of detections : %d", (int) detection.size());

	//object_tracker_with_new_frame(frame, detections);
	post_process(frame, detection);

	freq = cv::getTickFrequency() / 1000;
	t = get_net().getPerfProfile(layersTimes) / freq;
	label = cv::format("London South Bank University - Utku Bulkan - Frame processing time: %.2f ms", t);
	cv::putText(frame, label, cv::Point(0, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));

	syslog(LOG_DEBUG, "ObjectDetector_YoloTiny::process_frame End");
	return std::string("");
}
