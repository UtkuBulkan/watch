/*
 * Copyright 2019 SU Technology Ltd. All rights reserved.
 *
 * MIT License
 *
 * Copyright (c) 2019 SU Technology
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
 */
#include <syslog.h>
#include "camera_manager.h"

Camera::Camera(std::string input_device_name)
{
	m_input_device_name = input_device_name;

	capture.open(m_input_device_name);
	if ( !capture.isOpened	() ) {
		throw "Error opening file.\n";
	}
	cv::Mat frame;
	capture >> frame;

	int ex = static_cast<int>(capture.get(cv::CAP_PROP_FOURCC));	// Get Codec Type- Int form
	int codec = cv::VideoWriter::fourcc('M', 'P', 'G', '2');
	cv::Size S = cv::Size((int) capture.get(cv::CAP_PROP_FRAME_WIDTH), (int) capture.get(cv::CAP_PROP_FRAME_HEIGHT));

#ifdef CATDETECTOR_ENABLE_OUTPUT_TO_VIDEO_FILE
	outputVideo.open("./output.mp4", cv::CAP_FFMPEG, codec, capture.get(cv::CAP_PROP_FPS), S, true);
	outputVideo << frame;
#endif

	cv::namedWindow(m_input_device_name, cv::WINDOW_NORMAL);
	cv::resizeWindow(m_input_device_name, 640, 480);

	cv::namedWindow("Detected", cv::WINDOW_NORMAL);
	cv::resizeWindow("Detected", 640, 480);

	syslog(LOG_NOTICE, "Input file fourcc: %d, %d", codec, ex);
	syslog(LOG_NOTICE, "Input file width: %d", S.width);
	syslog(LOG_NOTICE, "Input file height: %d", S.height);

	syslog(LOG_NOTICE, "Device name : %s", m_input_device_name.c_str());
}

Camera::~Camera()
{
	outputVideo.release();
}

void Camera::display_statistics(cv::Mat &frame, std::string id, std::string gender, std::string age, cv::Point label_location)
{
	cv::putText(frame, cv::format("ID #%s", id), label_location, cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
	label_location.y += 25;
	cv::putText(frame, cv::format("%s", gender.c_str()), label_location, cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
	label_location.y += 25;
	cv::putText(frame, cv::format("%s", age.c_str()), label_location, cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
}

void Camera::loop(std::vector<ObjectDetector*> object_detectors, ObjectTracker *object_tracker)
{
	cv::Mat frame;

	int framecount = 0;

	while(1) {
		capture >> frame;
		syslog(LOG_NOTICE, "Frame count : %d", framecount);
		syslog(LOG_DEBUG, "Frame resolution : %d x %d", frame.rows, frame.cols);

		if (frame.empty()) {
			syslog(LOG_NOTICE, "Last read frame is empty, quitting.");
			break;
		}

		framecount++;
		{
			if (framecount % CATDETECTOR_SKIP_THIS_NUMBER_OF_FRAMES != 0)
				continue;
			/*if (framecount % CATDETECTOR_SKIP_THIS_NUMBER_OF_FRAMES == 0) {
				if(object_detector && object_tracker) {
					object_tracker->object_tracker_with_new_frame(frame, object_detector->process_frame(frame));
				} else if (object_detector) {
					object_detector->process_frame(frame);
				}
			} else {
				if(object_tracker) {
					object_tracker->object_tracker_update_only(frame);
				}
			}*/
			std::vector<std::pair<cv::Mat, cv::Point> > detected_faces;
			object_detectors[0]->process_frame(frame, detected_faces);
			for(size_t i = 0; i < detected_faces.size(); i++) {
				std::vector<std::pair<cv::Mat, cv::Point> > dummy;

				std::string gender = object_detectors[1]->process_frame(detected_faces[i].first, dummy);
				std::string age = object_detectors[2]->process_frame(detected_faces[i].first, dummy);
				cv::Point detection_label_location = detected_faces[i].second;
				cv::Point label_location = detected_faces[i].second;

				display_statistics(frame, std::to_string(i), gender, age, label_location);
				cv::imshow("Detected", detected_faces[i].first);
			}
			cv::putText(frame,
						cv::format("LSBU - frame # = %d", framecount),
						cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 1);

#ifdef CATDETECTOR_ENABLE_OUTPUT_TO_VIDEO_FILE
			/* Outputting captured frames to a video file */
			outputVideo << frame;
#endif
			cv::imshow(m_input_device_name, frame);

			/* Sending the data as a Kafka producer */
			/* video_analyser_kafka_producer(j.dump().c_str(), "TutorialTopic"); */
		}
		if(cv::waitKey(30) >= 0) break;
	}
}
