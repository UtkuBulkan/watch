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
*/

#ifndef OBJECTDETECTION_H
#define OBJECTDETECTION_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#define CATDETECTOR_ANALYSE_EVERY_24_FRAMES
//#define CATDETECTOR_ENABLE_OUTPUT_TO_VIDEO_FILE

class ObjectDetector {
public:
	static ObjectDetector* GenerateDetector(std::string objectdetector_type);

	void setup_model_for_detector(std::string class_definition_file, std::string model_config_file, std::string model_weights_file);
	void load_model_for_detector();

	std::vector < std::string > get_class_labels();
	float get_confidence_threshold();
	float get_nms_threshold();
	cv::dnn::Net get_net();

	virtual std::vector<cv::Mat> process_frame(cv::Mat &frame) = 0;
protected :
	ObjectDetector();
	~ObjectDetector();
	virtual void post_process(cv::Mat& frame, std::vector<cv::Mat> detection) = 0;
	virtual void draw_prediction_indicators(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame) = 0;

private:
	std::vector < std::string > classes;
	std::vector<cv::Scalar> colors;
	// Give the configuration and weight files for the model
	std::string colors_file;
	std::string m_class_definition_file;
	std::string m_model_config_file;
	std::string m_model_weights_file;

	// Initialize the parameters
	float m_confidence_threshold; // Confidence threshold
	float m_nms_threshold; // non-maximum suppression threshold

	cv::dnn::Net net;
};
#endif /* OBJECTDETECTION_H */
