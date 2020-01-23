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
#include "facerecognition.h"

FaceRecognition::FaceRecognition()
{
	syslog(LOG_NOTICE, "FaceRecognition::FaceRecognition Begin");
	model = cv::face::LBPHFaceRecognizer::create();
	//model = cv::face::FisherFaceRecognizer::create();
	last_detected_id = 5;
	face_recognition_model_database  = "./SVNDATABASE.cvs";
	face_image_list_csv = "../data/images/list.csv";
	load_face_recognition_model();
	syslog(LOG_NOTICE, "FaceRecognition::FaceRecognition End");
}

FaceRecognition::~FaceRecognition()
{
	syslog(LOG_NOTICE, "FaceRecognition::~FaceRecognition Begin");
	syslog(LOG_NOTICE, "FaceRecognition::~FaceRecognition Begin");
}

std::string FaceRecognition::getLabelInfo(int &predicted_id)
{
	return model->getLabelInfo(predicted_id);
}

void FaceRecognition::display_statistics(cv::Mat &frame, std::string id)
{
	syslog(LOG_NOTICE, "FaceRecognition::display_statistics Begin");
	cv::putText(frame, cv::format("%s", id.c_str()), cv::Size(1,20), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 0), 1);
	//cv::putText(frame, cv::format("C : %0.0lf", confidence), cv::Size(1,40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
	syslog(LOG_NOTICE, "FaceRecognition::display_statistics Begin");
}

void FaceRecognition::read_csv(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator = ';')
{
	syslog(LOG_NOTICE, " FaceRecognition::read_csv Begin");
	std::ifstream file(filename.c_str(), std::ifstream::in);
	if (!file) {
		syslog(LOG_NOTICE, "No valid face initial cvs list.");
		return ;
	}
	std::string line, path, classlabel;
	while (getline(file, line)) {
		std::stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if(!path.empty() && !classlabel.empty()) {
			cv::Mat image = cv::imread(path, 0);
			cv::resize(image,image,cv::Size(128,128));
			//cv::cvtColor(image, image, CV_GRAY2RGB);
			images.push_back(image);
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
	syslog(LOG_NOTICE, " FaceRecognition::read_csv End");
}

void FaceRecognition::load_face_recognition_model()
{
	syslog(LOG_NOTICE, "FaceRecognition::load_face_recognition_model Begin");
	std::vector<cv::Mat> images;
	std::vector<int> labels;

	try {
		read_csv(face_image_list_csv, images, labels);
	} catch (cv::Exception& e) {
		std::cerr << "Error opening file \"" << face_image_list_csv << "\". Reason: " << e.msg << std::endl;
		exit(1);
	}

	if(images.size() <= 1) {
		syslog(LOG_NOTICE, "This demo needs at least 2 images to work. Please add more images to your data set!");
	}
	syslog(LOG_NOTICE, "Initial Images loaded for face recognition database");

	std::ifstream file(face_recognition_model_database.c_str(), std::ifstream::in);
	if (file) {
		model->read(face_recognition_model_database);
		syslog(LOG_NOTICE, "face_recognition_model_database loaded");
	} else {
		syslog(LOG_NOTICE, "No valid face initial face database.");
		model->train(images, labels);
		model->setLabelInfo(0, "utku");
		model->setLabelInfo(1, "arnold");
		model->setLabelInfo(2, "ayleen");
		model->setLabelInfo(3, "maya");
		model->setLabelInfo(4, "eda");
		//model->write(face_recognition_model_database);
	}
	//model->setThreshold(0.0);
	cv::Mat testSample = images[0];
	int testLabel = labels[0];
	int predictedLabel = model->predict(testSample);
	syslog(LOG_NOTICE, "Predicted class = %d / Actual class = %d, Predicted string :%s", predictedLabel, testLabel, model->getLabelInfo(predictedLabel).c_str());
	syslog(LOG_NOTICE, "Get threshold :%lf", model->getThreshold());
	syslog(LOG_NOTICE, "FaceRecognition::load_face_recognition_model End");
}

std::string FaceRecognition::predict_new_sample(cv::Mat &detected_face, bool &previously_detected)
{
	syslog(LOG_NOTICE, "FaceRecognition::predict_new_sample End");
	int predicted_label_id;
	double confidence;
	model->predict(detected_face, predicted_label_id, confidence);
	std::string predicted_string = model->getLabelInfo(predicted_label_id);
	syslog(LOG_NOTICE, "Prediction : %d, Predicted string : %s, confidence : %lf", predicted_label_id, predicted_string.c_str(), confidence);

	if(confidence > 110.0) {
		predicted_string = cv::format("%d", train_new_sample(detected_face, -1));
		previously_detected = false;
	} else {
        if (trained_face_count.find(predicted_string) != trained_face_count.end()) {
            if (trained_face_count[predicted_string] < 50)  {
                trained_face_count[predicted_string] += 1;
                train_new_sample(detected_face, predicted_label_id);
            }
        } else {
            trained_face_count[predicted_string] = 1;
        }
        for(std::map <std::string, int>::iterator it=trained_face_count.begin(); it!=trained_face_count.end();it++) {
            syslog(LOG_NOTICE, "Face Index : %s - Count : %d", it->first.c_str(), it->second);
        }
		previously_detected = true;
	}

	/*std::string model_info = cv::format("\tLBPH(radius=%i, neighbors=%i, grid_x=%i, grid_y=%i, threshold=%.2f)",
			model->getInt("radius"),
			model->getInt("neighbors"),
			model->getInt("grid_x"),
			model->getInt("grid_y"),
			model->getDouble("threshold"));
	std::cout << model_info << std::endl;*/
	// We could get the histograms for example:
	//std::vector<cv::Mat> histograms = model->getMatVector("histograms");
	// But should I really visualize it? Probably the length is interesting:
	//std::cout << "Size of the histograms: " << histograms[0].total() << std::endl;
	syslog(LOG_NOTICE, "FaceRecognition::predict_new_sample End");
	return predicted_string;
}

int FaceRecognition::train_new_sample(cv::Mat &detected_face, int predicted_id)
{
	syslog(LOG_NOTICE, "FaceRecognition::train_new_sample Begin");
	int current_id;
	if (predicted_id == -1) {
		current_id = last_detected_id;
		last_detected_id++;
	} else {
		current_id = predicted_id;
	}
	std::vector<int> vec_id;
	vec_id.push_back(current_id);
	std::vector<cv::Mat> vec_image;
	vec_image.push_back(detected_face);
	model->update(vec_image, vec_id);
	std::ostringstream current_id_string_stream;
	current_id_string_stream << std::to_string(current_id);
	std::string current_id_string = current_id_string_stream.str();
	model->setLabelInfo(current_id, current_id_string);
	//model->write(face_recognition_model_database);
	syslog(LOG_NOTICE, "FaceRecognition::train_new_sample End");
	return current_id;
}

