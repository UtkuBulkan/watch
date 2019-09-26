/*
 * tracker.cpp
 *
 *  Created on: 17 Sep 2019
 *      Author: root
 */

#include <syslog.h>
#include "objecttracker.h"

std::vector<std::string> trackerTypes = {"BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};

ObjectTracker::ObjectTracker(std::string tracker_type) : m_tracker_type(tracker_type)
{
	multiTracker = cv::MultiTracker::create();
}

ObjectTracker::~ObjectTracker() { }

cv::Ptr<cv::Tracker> ObjectTracker::GenerateTracker(std::string tracker_type)
{
	cv::Ptr<cv::Tracker> tracker;
	if (tracker_type ==  trackerTypes[0])
		tracker = cv::TrackerBoosting::create();
	else if (tracker_type == trackerTypes[1])
		tracker =cv::TrackerMIL::create();
	else if (tracker_type == trackerTypes[2])
		tracker = cv::TrackerKCF::create();
	else if (tracker_type == trackerTypes[3])
		tracker = cv::TrackerTLD::create();
	else if (tracker_type == trackerTypes[4])
		tracker = cv::TrackerGOTURN::create();
	else if (tracker_type == trackerTypes[6])
		tracker = cv::TrackerMOSSE::create();
	else if (tracker_type == trackerTypes[7])
		tracker = cv::TrackerCSRT::create();
	else {
		std::cout << "Incorrect tracker name" << std::endl;
		std::cout << "Available trackers are: " << std::endl;
		for (std::vector<std::string>::iterator it = trackerTypes.begin() ; it != trackerTypes.end(); ++it)
			std::cout << " " << *it << std::endl;
	}
	return tracker;
}

int ObjectTracker::isObjectAlreadyTracked(std::vector<cv::Rect2d> tracked_objects, cv::Rect2d detected_object)
{
	for(size_t k=0; k<tracked_objects.size();k++) {
		syslog(LOG_NOTICE, "%d : Tracked.x = %lf, detected.x = %lf - Tracked.y = %lf, detected.y = %lf - Tracked.height = %lf, detected.height = %lf - Tracked.width = %lf, detected.width = %lf",
						(int)k, tracked_objects[k].x, detected_object.x,
							tracked_objects[k].y, detected_object.y,
							tracked_objects[k].height, detected_object.height,
							tracked_objects[k].width, detected_object.width);
		if( fabs(tracked_objects[k].x - detected_object.x) <= PIXEL_CONFIDENCE_BORDER &&
			fabs(tracked_objects[k].y - detected_object.y) <= PIXEL_CONFIDENCE_BORDER &&
			fabs(tracked_objects[k].height - detected_object.height) <= PIXEL_CONFIDENCE_BORDER &&
			fabs(tracked_objects[k].width - detected_object.width) <= PIXEL_CONFIDENCE_BORDER ) {
			return true;
		}
	}
	return false;
}

void ObjectTracker::object_tracker_update_only(cv::Mat &frame)
{
	multiTracker->update(frame);

	std::vector<cv::Rect2d> tracked_objects = multiTracker->getObjects();

	for(unsigned i=0; i<tracked_objects.size(); i++)
	{
		cv::rectangle(frame, multiTracker->getObjects()[i], 5, 2, 1);
	}
}

void ObjectTracker::object_tracker_with_new_frame(cv::Mat &frame, std::vector<cv::Mat> detections)
{
	syslog(LOG_NOTICE, "ObjectDetector::object_tracker Begin");

	std::vector<cv::Rect2d> tracked_objects = multiTracker->getObjects();

	multiTracker->update(frame);
	for(size_t i=0;i<detections.size();i++) {
		// Scan through all the bounding boxes output from the network and keep only the
		// ones with high confidence scores. Assign the box's class label as the class
		// with the highest score for the box.
		float* data = (float*)detections[i].data;
		for (int j = 0; j < detections[i].rows; ++j, data += detections[i].cols)
		{
			cv::Mat scores = detections[i].row(j).colRange(5, detections[i].cols);
			cv::Point classIdPoint;
			double confidence;
			// Get the value and location of the maximum score
			cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
			if (confidence > 0.5)
			{
				int centerX = (int)(data[0] * frame.cols);
				int centerY = (int)(data[1] * frame.rows);
				int width = (int)(data[2] * frame.cols);
				int height = (int)(data[3] * frame.rows);
				int left = centerX - width / 2;
				int top = centerY - height / 2;

				if(!isObjectAlreadyTracked(tracked_objects, cv::Rect2d(left, top, width, height)))
					multiTracker->add(GenerateTracker(trackerTypes[2]), frame, cv::Rect2d(left, top, width, height));
			}
		}
	}
	std::vector<cv::Scalar> colors;
	getRandomColors(colors, detections.size());

	for(unsigned i=0; i<tracked_objects.size(); i++)
	{
		cv::rectangle(frame, tracked_objects[i], colors[i], 2, 1);
	}
	syslog(LOG_NOTICE, "ObjectDetector::object_tracker End");
}

// Fill the vector with random colors

void ObjectTracker::getRandomColors(std::vector<cv::Scalar>& colors, int numColors)
{
	cv::RNG rng(0);
	for(int i=0; i < numColors; i++)
	colors.push_back(cv::Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255)));
}
