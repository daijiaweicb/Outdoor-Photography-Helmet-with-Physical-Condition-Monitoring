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

    
    const float EAR_THRESHOLD = 0.30f;
    const int EYES_CLOSED_FRAMES = 15;
    int counter = 0;

   
    int frame_count = 0; 
    bool has_face = false; 
    cv::Rect last_face;

    bool landmark_initialized = false;
    std::vector<cv::Point2f> prev_left_eye; 
    std::vector<cv::Point2f> prev_right_eye; 

    float eye_aspect_ratio(const std::vector<cv::Point2f>& eye);
    std::vector<cv::Point2f> extract_eye(const dlib::full_object_detection& shape, bool left);
};

#endif // FATIGUE_DETECTOR_H
