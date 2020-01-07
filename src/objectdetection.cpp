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
