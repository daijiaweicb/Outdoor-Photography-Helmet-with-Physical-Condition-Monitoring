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
    FatigueDetector();
    bool detect(const cv::Mat& frame, cv::Mat& output);

private:
    float eyeAspectRatio(const std::vector<dlib::point>& eye);
    float mouth_aspect_ratio(const std::vector<cv::Point>& mouth);

    std::map<std::string, double> calculateEBBA(double ear, double eyeClosedDuration);
    std::map<std::string, double> calculateMBBA(double mar, double yawnDuration);
    std::map<std::string, double> combineBBA(const std::map<std::string, double>& bba1, const std::map<std::string, double>& bba2);

    dlib::frontal_face_detector detector;
    dlib::shape_predictor predictor;

    cv::KalmanFilter KF;
    cv::Mat_<float> measurement;
    bool kalmanInitialized = false;

    std::chrono::high_resolution_clock::time_point lastBlinkStart, lastYawnStart;
    bool eyeClosed = false;
    bool yawnDetected = false;
    double eyeClosedDuration = 0.0;
    double yawnDuration = 0.0;

    std::map<std::string, double> prevEBBA;
    std::map<std::string, double> prevMBBA;

    const double EAR_DANGER_THRESHOLD = 0.16;
    const double EAR_WARNING_THRESHOLD = 0.22;
    const double MAR_YAWN_THRESHOLD = 0.78;
    const double MAR_SPEAK_THRESHOLD = 0.5;
    const double EYE_DURATION_THRESHOLD = 1.5;
    const double YAWN_DURATION_THRESHOLD = 3.0;
    const double HIGH_FATIGUE_THRESHOLD = 0.8;
};

#endif // FATIGUE_DETECTOR_H
