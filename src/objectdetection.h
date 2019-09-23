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
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
//#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tracker.hpp>
#include <string>
#include "json.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <string>


using json = nlohmann::json;

class ObjectDetector {
public:
	ObjectDetector();
	~ObjectDetector();
	void process_frame(cv::Mat &frame, int framecount, std::string hash_video);
	void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame);
	void post_process(cv::Mat& frame, const std::vector<cv::Mat>& outs, int framecount, std::string hash_video);
	void loop();
	//void generate_json(cv::Mat &frame, const int &classId, const int &framecount, std::string frame_md5, std::string video_md5);
	std::vector<std::string> getOutputsNames();
	void object_tracker_with_new_frame(cv::Mat &frame, std::vector<cv::Mat> outs);
	void object_tracker_update_only(cv::Mat &frame);

	std::string filename;
private:
	// Initialize the parameters
	float confidence_threshold; // Confidence threshold
	float nmsThreshold; // non-maximum suppression threshold
	cv::dnn::Net net;
	std::vector < std::string > classes;
	std::vector<cv::Scalar> colors;
	// Give the configuration and weight files for the model
	std::string class_definition_file;
	std::string colors_file;
	std::string model_config_file;
	std::string model_weights_file;

	cv::Ptr<cv::MultiTracker> multiTracker;

	json j;
};
