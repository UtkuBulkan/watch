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
#include <sys/time.h>

#include "objectdetection_ssdcaffe.h"

#define inHeight 300
#define inWidth 300

ObjectDetector_SsdCaffe::ObjectDetector_SsdCaffe()
{
    syslog ( LOG_NOTICE, "ObjectDetector_SsdCaffe Constructor Begin" );
    setup_model_for_detector ( "", "../data/ssdcaffe/deploy.prototxt", "../data/ssdcaffe/res10_300x300_ssd_iter_140000_fp16.caffemodel" );
    load_network_model_for_detector ( "Caffe" );
    syslog ( LOG_NOTICE, "ObjectDetector_SsdCaffe Constructor End" );
}

ObjectDetector_SsdCaffe::~ObjectDetector_SsdCaffe()
{
}

void ObjectDetector_SsdCaffe::draw_prediction_indicators ( int classId, float confidence, int left, int top, int right, int bottom, cv::Mat& frame )
{
    ( void ) classId;
    ( void ) confidence;
    ( void ) left;
    ( void ) top;
    ( void ) right;
    ( void ) bottom;
    ( void ) frame;
    syslog ( LOG_NOTICE, "ObjectDetector_SsdCaffe::draw_box Begin" );
    syslog ( LOG_NOTICE, "ObjectDetector_SsdCaffe::draw_box End" );
}

void ObjectDetector_SsdCaffe::post_process ( cv::Mat& frame, std::vector<cv::Mat> detection )
{
    ( void ) frame;
    ( void ) detection;
    syslog ( LOG_NOTICE, "ObjectDetector_SsdCaffe::post_process Begin" );
    syslog ( LOG_NOTICE, "ObjectDetector_SsdCaffe::post_process End" );
}

long long current_timestamp()
{
    struct timeval te;
    gettimeofday ( &te, NULL );
    long long  miliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return miliseconds;
}

std::string ObjectDetector_SsdCaffe::process_frame ( cv::Mat &frame, std::vector<std::pair<cv::Mat, cv::Point> > &detections )
{
    syslog ( LOG_NOTICE, "ObjectDetector_SsdCaffe::process_frame Begin" );

    //long long time_start = current_timestamp();

    const cv::Scalar meanVal ( 104.0, 177.0, 123.0 );
    const double inScaleFactor = 1.0;
    int frameHeight = frame.rows;
    int frameWidth = frame.cols;
    cv::Mat inputBlob = cv::dnn::blobFromImage ( frame, inScaleFactor, cv::Size ( frameHeight, frameWidth ), meanVal, false, false );

    get_net().setInput ( inputBlob, "data" );
    cv::Mat detection = get_net().forward ( "detection_out" );

    cv::Mat detectionMat ( detection.size[2], detection.size[3], CV_32F, detection.ptr<float>() );

    for ( int i = 0; i < detectionMat.rows; i++ ) {
        float confidence = detectionMat.at<float> ( i, 2 );

        if ( confidence > get_confidence_threshold() ) {
            int x1 = static_cast<int> ( detectionMat.at<float> ( i, 3 ) * frameWidth );
            int y1 = static_cast<int> ( detectionMat.at<float> ( i, 4 ) * frameHeight );
            int x2 = static_cast<int> ( detectionMat.at<float> ( i, 5 ) * frameWidth );
            int y2 = static_cast<int> ( detectionMat.at<float> ( i, 6 ) * frameHeight );

            int face_width = x2 - x1;
            int face_height = y2 - y1;

            int face_width_padding = face_width * 1.0/6.0;
            int face_height_padding = face_height * 1.0/4.0;

            x1 = x1 - face_width_padding;
            x2 = x2 + face_width_padding;
            y1 = y1 - face_height_padding;
            y2 = y2 + face_height_padding;

            x1 = std::max ( x1, 0 );
            y1 = std::max ( y1, 0 );
            x2 = std::min ( x2, frameWidth-1 );
            y2 = std::min ( y2, frameHeight-1 );

            face_width = x2 - x1;
            face_height = y2 - y1;

            syslog ( LOG_NOTICE, "%d - %d,%d,%d,%d, framewidth : %d, frameheight : %d",
                     i, x1, y1, face_width, face_height, frameWidth, frameHeight );

            if ( 0 <= x1 && x1 < frameWidth &&
                    0 <= y1 && y1 < frameHeight &&
                    0 < face_width && x2 < frameWidth &&
                    0 < face_height && y2 < frameHeight ) {
                cv::Mat face_detected;
                frame ( cv::Rect ( x1,y1,face_width,face_height ) ).copyTo ( face_detected );
                detections.push_back ( std::make_pair ( face_detected, cv::Point ( x2+3,y1+22 ) ) );

                cv::rectangle ( frame, cv::Point ( x1, y1 ), cv::Point ( x2, y2 ), cv::Scalar ( 0, 255, 0 ),2, 1 );
            }
        }
    }

    //long long time_end= current_timestamp();

    //double fps = 1.0/(time_end - time_start);
    //cv::putText(frame,
    //		cv::format("LSBU - Utku Bulkan - freame = %.2lf", fps),
    //		cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.4, cv::Scalar(0, 0, 255), 4);

    syslog ( LOG_NOTICE, "ObjectDetector_SsdCaffe::process_frame End" );

    return std::string ( "" );
}
