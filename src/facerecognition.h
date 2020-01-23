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
#include <string>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include <fstream>
#include <map>

class FaceRecognition
{
public:
	FaceRecognition();
	~FaceRecognition();
	void load_face_recognition_model();
	std::string getLabelInfo(int &predicted_id);
	std::string predict_new_sample(cv::Mat &detected_face, bool &previously_detected);
	int train_new_sample(cv::Mat &detected_face, int predicted_id);
	void display_statistics(cv::Mat &frame, std::string predicted_string);
private:
	std::string face_recognition_model_database;
	std::string face_image_list_csv;

	cv::Ptr<cv::face::FaceRecognizer> model;
	void read_csv(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator);

	int last_detected_id;
    
    std::map<std::string, int> trained_face_count;
};
