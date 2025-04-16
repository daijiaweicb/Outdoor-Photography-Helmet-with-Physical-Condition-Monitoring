#ifndef FATIGUE_DETECTOR_H
#define FATIGUE_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>             
#include <dlib/opencv.h>
#include <dlib/image_processing.h>

class FatigueDetector {
public:
    FatigueDetector();

    
    bool detect(const cv::Mat& frame, cv::Mat& output);

private:
    dlib::shape_predictor predictor; 
    cv::dnn::Net face_net; 

    
    float eye_aspect_ratio(const std::vector<cv::Point2f>& eye);

    
    std::vector<cv::Point2f> extract_eye(const dlib::full_object_detection& shape, bool left);

    
    const float EAR_THRESHOLD = 0.25f;     
    const int EYES_CLOSED_FRAMES = 15;     
    int counter = 0;                      
};

#endif 
