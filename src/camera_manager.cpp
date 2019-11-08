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
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <chrono>
#include <syslog.h>
#include "camera_manager.h"

Camera::Camera(std::string input_device_name, CameraSettingsData &camera_settings_data)
{
	syslog(LOG_NOTICE, "Camera::Camera Begin");
	m_input_device_name = input_device_name;

	m_camera_settings_data = camera_settings_data;

	capture.open(m_input_device_name);
	if ( !capture.isOpened	() ) {
		throw "Error opening file.\n";
	}
	framecount = 0;
	syslog(LOG_NOTICE, "Camera::Camera End");
}

Camera::~Camera()
{
	syslog(LOG_NOTICE, "Camera::~Camera Begin");
	outputVideo.release();
	syslog(LOG_NOTICE, "Camera::~Camera End");
}

void Camera::enable_recording_as_output_file()
{
	int ex = static_cast<int>(capture.get(cv::CAP_PROP_FOURCC));	// Get Codec Type- Int form
	int codec = cv::VideoWriter::fourcc('M', 'P', 'G', '2');
	cv::Size S = cv::Size((int) capture.get(cv::CAP_PROP_FRAME_WIDTH), (int) capture.get(cv::CAP_PROP_FRAME_HEIGHT));

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream output_file_path_string_stream;
	output_file_path_string_stream << "./recordings/" << m_input_device_name << "-" << std::put_time(&tm, "%d-%m-%Y-%H-%M-%S") << ".mp4";
	m_output_file_path = output_file_path_string_stream.str();

	outputVideo.open(m_output_file_path, cv::CAP_FFMPEG, codec, m_fps, S, true);

	syslog(LOG_NOTICE, "Input file fourcc: %d, %d", codec, ex);
	syslog(LOG_NOTICE, "Input file width: %d", S.width);
	syslog(LOG_NOTICE, "Input file height: %d", S.height);
	syslog(LOG_NOTICE, "Device name : %s", m_input_device_name.c_str());
}

void Camera::display_statistics(cv::Mat &frame, std::string id, std::string gender, std::string age, cv::Point label_location)
{
	syslog(LOG_NOTICE, "Camera::display_statistics Begin");
	if (m_camera_settings_data.face_recognition > 0) {
		cv::putText(frame, cv::format("%s", id.c_str()), label_location, cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
		label_location.y += 25;
	}
	if (m_camera_settings_data.gender_prediction > 0) {
		cv::putText(frame, cv::format("%s", gender.c_str()), label_location, cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
		label_location.y += 25;
	}
	if (m_camera_settings_data.age_prediction > 0) {
		cv::putText(frame, cv::format("%s", age.c_str()), label_location, cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
		syslog(LOG_NOTICE, "Camera::display_statistics End");
	}
}

void Camera::set_models(std::vector<ObjectDetector*> object_detectors, ObjectTracker *object_tracker, FaceRecognition *face_recognitor)
{
	m_object_detectors = object_detectors;
	m_object_tracker = object_tracker;
	m_face_recognitor = face_recognitor;
}

std::string Camera::get_input_device_name()
{
	return m_input_device_name;
}

void Camera::process_frame()
{
	syslog(LOG_NOTICE, "Camera::loop Begin");

	{
		capture >> frame;

		if (frame.empty()) {
			syslog(LOG_NOTICE, "Last read frame is empty, quitting.");
			return; //break;
		}

		if(framecount == 0) {
			m_fps = capture.get(cv::CAP_PROP_FPS);
			syslog(LOG_NOTICE, "Frame count : %d", framecount);
			syslog(LOG_DEBUG, "Frame resolution : %d x %d", frame.rows, frame.cols);
		}

		framecount++;
		if (framecount % CATDETECTOR_SKIP_THIS_NUMBER_OF_FRAMES != 0) {
			return; //continue;
		}

		{
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
			int64_t start_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			if (m_camera_settings_data.face_detection > 0) {
				std::vector<std::pair<cv::Mat, cv::Point> > detected_faces;
				m_object_detectors[0]->process_frame(frame, detected_faces);

				std::string gender;
				std::string age;
				std::string predicted_string;

				for(size_t i = 0; i < detected_faces.size(); i++) {
					std::vector<std::pair<cv::Mat, cv::Point> > dummy;

					if (m_camera_settings_data.gender_prediction > 0) {
						gender = m_object_detectors[1]->process_frame(detected_faces[i].first, dummy);
					}
					if (m_camera_settings_data.age_prediction > 0) {
						age = m_object_detectors[2]->process_frame(detected_faces[i].first, dummy);
					}
					cv::Point label_location = detected_faces[i].second;

					cv::Mat grayscale;
					cv::cvtColor(detected_faces[i].first, grayscale, CV_RGB2GRAY);
					cv::resize(grayscale,grayscale,cv::Size(128,128));

					if (m_camera_settings_data.face_recognition > 0) {
						bool previously_detected;
						predicted_string = m_face_recognitor->predict_new_sample(grayscale, previously_detected);
						m_face_recognitor->display_statistics(detected_faces[i].first, predicted_string);

						if(!previously_detected) {
							QImage qimage_detected_face(detected_faces[i].first.data, detected_faces[i].first.cols, detected_faces[i].first.rows,
									detected_faces[i].first.step, QImage::Format_RGB888);
							emit loop_add_detected_face(qimage_detected_face);
						}
					}
					display_statistics(frame, predicted_string, gender, age, label_location);
				}
			}
			int64_t end_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

			if(start_time && end_time) {
				double momenteraly_fps = 1.0 / ((end_time - start_time)/1000000000.0);
				overall_fps = ((framecount-1) * overall_fps + momenteraly_fps) / (framecount);
			}

			cv::putText(frame, cv::format("LSBU-F#%d,fps#%2.2lf,video_fps=%d", framecount, overall_fps, (int)m_fps), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
			QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
			emit loop_set_pixmap(qimg,  QString::fromStdString(m_input_device_name));

			if (m_camera_settings_data.record_detections_as_output_file > 0) {
				if(m_output_file_path.empty()) {
					enable_recording_as_output_file();
				}
				outputVideo << frame;
			}
		}
	}
	syslog(LOG_NOTICE, "Camera::loop End");
}
