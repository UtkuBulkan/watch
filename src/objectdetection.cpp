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

#include "tensorflow/c/c_api.h"
#include "objectdetection.h"
#include "md5_helper.h"

#define CATDETECTOR_ANALYSE_EVERY_24_FRAMES
#define CATDETECTOR_ENABLE_OUTPUT_TO_VIDEO_FILE
#define CATDETECTOR_ENABLE_CAPTURED_FRAMES_TO_JSON

#define inpHeight 416
#define inpWidth 416

#define DEEP_LEARNING_MODEL_YOLO_TINY
//#define DEEP_LEARNING_MODEL_YOLO

ObjectDetector::ObjectDetector() : confidence_threshold(0.5),
	nmsThreshold(0.4),
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
	net = cv::dnn::readNetFromDarknet(model_config_file, model_weights_file);
	net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
	syslog (LOG_NOTICE, "ObjectDetector Network Loaded");

	multiTracker = cv::MultiTracker::create();

	syslog (LOG_NOTICE, "ObjectDetector Constructor End");
}

ObjectDetector::~ObjectDetector() {
}

void ObjectDetector::drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame)
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

#if 0
void ObjectDetector::generate_json(cv::Mat &frame, const int &classId, const int &framecount, std::string frame_md5, std::string video_md5)
{
	syslog(LOG_NOTICE, "ObjectDetector::generate_json Begin");

	json local_j;

	local_j["class"] = classes[classId].c_str();
	local_j["frame"] = framecount;
	local_j["hash-frame"] = frame_md5;
	local_j["hash-video"] = video_md5;

	std::vector<uchar> buffer;
#define MB 1024 * 1024
	buffer.resize(200*MB);
	cv::imencode(".png", frame, buffer);
	local_j["image"] = buffer;
	j.push_back(local_j);

	syslog(LOG_NOTICE, "ObjectDetector::generate_json End");
}
#endif

void ObjectDetector::post_process(cv::Mat& frame, const std::vector<cv::Mat>& outs, int framecount, std::string hash_video)
{
	syslog(LOG_NOTICE, "ObjectDetector::post_process Begin");

	std::vector<int> classIds;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;

	for (size_t i = 0; i < outs.size(); ++i)
	{
		// Scan through all the bounding boxes output from the network and keep only the
		// ones with high confidence scores. Assign the box's class label as the class
		// with the highest score for the box.
		float* data = (float*)outs[i].data;
		for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
		{
			cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
			cv::Point classIdPoint;
			double confidence;
			// Get the value and location of the maximum score
			cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
			if (confidence > confidence_threshold)
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

				//generate_json(frame, classIds[i], framecount, "", hash_video);
			}
		}
	}

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

std::vector<std::string> trackerTypes = {"BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};

cv::Ptr<cv::Tracker> createTrackerByName(std::string trackerType)
{
	cv::Ptr<cv::Tracker> tracker;
	if (trackerType ==  trackerTypes[0])
		tracker = cv::TrackerBoosting::create();
	else if (trackerType == trackerTypes[1])
		tracker =cv::TrackerMIL::create();
	else if (trackerType == trackerTypes[2])
		tracker = cv::TrackerKCF::create();
	else if (trackerType == trackerTypes[3])
		tracker = cv::TrackerTLD::create();
	else if (trackerType == trackerTypes[4])
		tracker = cv::TrackerGOTURN::create();
	else if (trackerType == trackerTypes[6])
		tracker = cv::TrackerMOSSE::create();
	else if (trackerType == trackerTypes[7])
		tracker = cv::TrackerCSRT::create();
	else {
		std::cout << "Incorrect tracker name" << std::endl;
		std::cout << "Available trackers are: " << std::endl;
		for (std::vector<std::string>::iterator it = trackerTypes.begin() ; it != trackerTypes.end(); ++it)
			std::cout << " " << *it << std::endl;
	}
	return tracker;
}
// Fill the vector with random colors

void getRandomColors(std::vector<cv::Scalar>& colors, int numColors)
{
	cv::RNG rng(0);
	for(int i=0; i < numColors; i++)
	colors.push_back(cv::Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255)));
}

int isObjectAlreadyTracked(std::vector<cv::Rect2d> tracked_objects, cv::Rect2d detected_object)
{
	for(size_t k=0; k<tracked_objects.size();k++) {
		if( abs(tracked_objects[k].x - detected_object.x) <= 5 &&
			abs(tracked_objects[k].y - detected_object.y) <= 5 &&
			abs(tracked_objects[k].height - detected_object.height) <= 5 &&
			abs(tracked_objects[k].width - detected_object.width) <= 5 ) {
			return true;
		}
	}
	return false;
}

void ObjectDetector::object_tracker_update_only(cv::Mat &frame)
{
	multiTracker->update(frame);

	std::vector<cv::Rect2d> tracked_objects = multiTracker->getObjects();

	for(unsigned i=0; i<tracked_objects.size(); i++)
	{
		cv::rectangle(frame, multiTracker->getObjects()[i], 5, 2, 1);
	}
}

void ObjectDetector::object_tracker_with_new_frame(cv::Mat &frame, std::vector<cv::Mat> outs)
{
	syslog(LOG_NOTICE, "ObjectDetector::object_tracker Begin");

	std::vector<cv::Rect2d> tracked_objects = multiTracker->getObjects();

	multiTracker->update(frame);
	for(size_t i=0;i<outs.size();i++) {
		// Scan through all the bounding boxes output from the network and keep only the
		// ones with high confidence scores. Assign the box's class label as the class
		// with the highest score for the box.
		float* data = (float*)outs[i].data;
		for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
		{
			cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
			cv::Point classIdPoint;
			double confidence;
			// Get the value and location of the maximum score
			cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
			if (confidence > confidence_threshold)
			{
				int centerX = (int)(data[0] * frame.cols);
				int centerY = (int)(data[1] * frame.rows);
				int width = (int)(data[2] * frame.cols);
				int height = (int)(data[3] * frame.rows);
				int left = centerX - width / 2;
				int top = centerY - height / 2;

				if(!isObjectAlreadyTracked(tracked_objects, cv::Rect2d(left, top, width, height)))
					multiTracker->add(createTrackerByName("CSRT"), frame, cv::Rect2d(left, top, width, height));
			}
		}
	}
	std::vector<cv::Scalar> colors;
	getRandomColors(colors, outs.size());

	for(unsigned i=0; i<tracked_objects.size(); i++)
	{
		cv::rectangle(frame, tracked_objects[i], colors[i], 2, 1);
	}
	syslog(LOG_NOTICE, "ObjectDetector::object_tracker End");
}

void ObjectDetector::process_frame(cv::Mat &frame, int framecount, std::string hash_video) {
	std::vector<std::string> outNames(2);
	std::vector<double> layersTimes;
	std::string label;
	cv::Mat blob;
	double freq, t;

	syslog(LOG_NOTICE, "ObjectDetector::process_frame Begin");

	//cv::dnn::blobFromImage(frame, blob);
	cv::dnn::blobFromImage(frame, blob, 1/255.0, cv::Size(inpWidth, inpHeight), cv::Scalar(), true, false);
	net.setInput(blob);

	std::vector<cv::Mat> outs;
	net.forward(outs, getOutputsNames());

	syslog(LOG_NOTICE, "Number of outs : %d", (int) outs.size());

	object_tracker_with_new_frame(frame, outs);

	post_process(frame, outs, framecount, hash_video);
	freq = cv::getTickFrequency() / 1000;
	t = net.getPerfProfile(layersTimes) / freq;
	label = cv::format("London South Bank University - Utku Bulkan - Frame processing time: %.2f ms", t);
	cv::putText(frame, label, cv::Point(0, 20), cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 0, 0));


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
	std::string hash_video = md5_hash(filename);

	while(1) {
		syslog(LOG_NOTICE, "Frame count : %d", framecount);
		syslog(LOG_NOTICE, "Frame resolution : %d x %d", frame.rows, frame.cols);

		capture >> frame;
		if (frame.empty()) {
			syslog(LOG_NOTICE, "Last read frame is empty, quitting.");
			break;
		}

		framecount++;
#ifdef CATDETECTOR_ANALYSE_EVERY_24_FRAMES
		{
			if (framecount % 24 == 0)
				process_frame(frame, framecount, hash_video);
#endif
			object_tracker_update_only(frame);
#ifdef CATDETECTOR_ENABLE_OUTPUT_TO_VIDEO_FILE
			/* Outputting captured frames to a video file */
			outputVideo << frame;
#endif
			cv::imshow("Camera1", frame);

#ifdef CATDETECTOR_ENABLE_CAPTURED_FRAMES_TO_JSON
			/* Outputting captured frames to json */
			std::ofstream myfile;
			std::string videodata_filename(hash_video + ".json");
			myfile.open (videodata_filename);
			myfile << j << std::endl;
			myfile.close();
#endif
			/* Sending the data as a Kafka producer */
			/* video_analyser_kafka_producer(j.dump().c_str(), "TutorialTopic"); */
		}
		if(cv::waitKey(30) >= 0) break;
	}
	outputVideo.release();
}
