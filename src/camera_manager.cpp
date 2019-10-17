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

Camera::Camera(std::string input_device_name, CameraSettingsData &camera_settings_data)
{
	syslog(LOG_NOTICE, "Camera::Camera Begin");
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

	syslog(LOG_NOTICE, "Input file fourcc: %d, %d", codec, ex);
	syslog(LOG_NOTICE, "Input file width: %d", S.width);
	syslog(LOG_NOTICE, "Input file height: %d", S.height);

	syslog(LOG_NOTICE, "Device name : %s", m_input_device_name.c_str());
	syslog(LOG_NOTICE, "Camera::Camera End");
}

Camera::~Camera()
{
	syslog(LOG_NOTICE, "Camera::~Camera Begin");
	outputVideo.release();
	syslog(LOG_NOTICE, "Camera::~Camera End");
}

void Camera::display_statistics(cv::Mat &frame, std::string id, std::string gender, std::string age, cv::Point label_location)
{
	syslog(LOG_NOTICE, "Camera::display_statistics Begin");
	cv::putText(frame, cv::format("ID #%s", id.c_str()), label_location, cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
	label_location.y += 25;
	cv::putText(frame, cv::format("%s", gender.c_str()), label_location, cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
	label_location.y += 25;
	cv::putText(frame, cv::format("%s", age.c_str()), label_location, cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
	syslog(LOG_NOTICE, "Camera::display_statistics End");
}

void Camera::set_models(std::vector<ObjectDetector*> object_detectors, ObjectTracker *object_tracker, FaceRecognition *face_recognitor)
{
	m_object_detectors = object_detectors;
	m_object_tracker = object_tracker;
	m_face_recognitor = face_recognitor;
}

void Camera::camera_set_ui(MainWindow *main_window)
{
	m_main_window = main_window;
}

void Camera::loop()
{
	syslog(LOG_NOTICE, "Camera::loop Begin");
	cv::Mat frame;
	int framecount = 0;

	while(1) {
		capture >> frame;

		if(framecount == 0) {
			syslog(LOG_NOTICE, "Frame count : %d", framecount);
			syslog(LOG_DEBUG, "Frame resolution : %d x %d", frame.rows, frame.cols);
		}

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
			m_object_detectors[0]->process_frame(frame, detected_faces);
			for(size_t i = 0; i < detected_faces.size(); i++) {
				std::vector<std::pair<cv::Mat, cv::Point> > dummy;

				std::string gender = m_object_detectors[1]->process_frame(detected_faces[i].first, dummy);
				std::string age = m_object_detectors[2]->process_frame(detected_faces[i].first, dummy);
				cv::Point label_location = detected_faces[i].second;

				cv::Mat grayscale;
				cv::cvtColor(detected_faces[i].first, grayscale, CV_RGB2GRAY);
				cv::resize(grayscale,grayscale,cv::Size(128,128));

				bool previously_detected;
				std::string predicted_string = m_face_recognitor->predict_new_sample(grayscale, previously_detected);
				m_face_recognitor->display_statistics(detected_faces[i].first, predicted_string);
				//cv::imshow("Detected", detected_faces[i].first);
				if(!previously_detected) {
					QImage qimage_detected_face(detected_faces[i].first.data, detected_faces[i].first.cols, detected_faces[i].first.rows,
							detected_faces[i].first.step, QImage::Format_RGB888);
					m_main_window->add_detected_face(qimage_detected_face);
				}

				display_statistics(frame, predicted_string, gender, age, label_location);
			}
			cv::putText(frame, cv::format("LSBU - frame # %d", framecount), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);

#ifdef CATDETECTOR_ENABLE_OUTPUT_TO_VIDEO_FILE
			/* Outputting captured frames to a video file */
			outputVideo << frame;
#endif
			//cv::imshow(m_input_device_name, frame);

			QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
			m_main_window->setPixmap(qimg);

			/* Sending the data as a Kafka producer */
			/* video_analyser_kafka_producer(j.dump().c_str(), "TutorialTopic"); */
		}
		if(cv::waitKey(30) >= 0) break;
	}
	syslog(LOG_NOTICE, "Camera::loop End");
}
