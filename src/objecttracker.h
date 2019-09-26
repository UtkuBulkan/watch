/*
 * tracker.cpp
 *
 *  Created on: 17 Sep 2019
 *      Author: root
 */

#ifndef OBJECTTRACKER_H
#define OBJECTTRACKER_H

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

#define PIXEL_CONFIDENCE_BORDER 70

class ObjectTracker {
public:
	cv::Ptr<cv::Tracker> GenerateTracker(std::string tracker_type);

	ObjectTracker(std::string tracker_type);
	~ObjectTracker();
	void object_tracker_with_new_frame(cv::Mat &frame, std::vector<cv::Mat> outs);
	void object_tracker_update_only(cv::Mat &frame);
	int isObjectAlreadyTracked(std::vector<cv::Rect2d> tracked_objects, cv::Rect2d detected_object);
private:
	void getRandomColors(std::vector<cv::Scalar>& colors, int numColors);

	cv::Ptr<cv::MultiTracker> multiTracker;
	std::string m_tracker_type;
};
#endif /* OBJECTTRACKER_H */
