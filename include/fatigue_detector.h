#ifndef FATIGUE_DETECTOR_H
#define FATIGUE_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <map>
#include <string>
#include <chrono>

class FatigueDetector {
public:
    FatigueDetector(const std::string& model_path = "models/shape_predictor_68_face_landmarks.dat");
    bool detect(const cv::Mat& frame, cv::Mat& output);

private:
    float eye_aspect_ratio(const std::vector<cv::Point2f>& eye);
    
    std::vector<cv::Point2f> extract_eye(const dlib::full_object_detection& shape, bool left);

    dlib::shape_predictor predictor;
    dlib::frontal_face_detector face_detector;

    std::chrono::high_resolution_clock::time_point lastBlinkStart;
    bool eyeClosed;
    double eyeClosedDuration;

    cv::KalmanFilter KF;
    bool kalmanInitialized = false;

    const float EAR_THRESHOLD = 0.21f;
};

#endif // FATIGUE_DETECTOR_H