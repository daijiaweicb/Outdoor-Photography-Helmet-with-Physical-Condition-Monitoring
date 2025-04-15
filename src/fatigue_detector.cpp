#include "fatigue_detector.h"
#include <iostream>
#include <chrono>

FatigueDetector::FatigueDetector() {
    face_detector = dlib::get_frontal_face_detector();

    try {
        dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> predictor;
    } catch (std::exception &e) {
        std::cerr << "Failed to load shape predictor: " << e.what() << std::endl;
    }

    eyeClosed = false;
    eyeClosedDuration = 0.0;

    KF.init(4, 2, 0);
    KF.transitionMatrix = (cv::Mat_<float>(4, 4) << 1, 0, 1, 0,
                                                     0, 1, 0, 1,
                                                     0, 0, 1, 0,
                                                     0, 0, 0, 1);
    setIdentity(KF.measurementMatrix);
    setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-5));
    setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-1));
    setIdentity(KF.errorCovPost, cv::Scalar::all(1));
}

float FatigueDetector::eye_aspect_ratio(const std::vector<cv::Point2f>& eye) {
    float A = cv::norm(eye[1] - eye[5]);
    float B = cv::norm(eye[2] - eye[4]);
    float C = cv::norm(eye[0] - eye[3]);
    return (A + B) / (2.0f * C);
}

std::vector<cv::Point2f> FatigueDetector::extract_eye(const dlib::full_object_detection& shape, bool left) {
    std::vector<cv::Point2f> eye;
    int start = left ? 36 : 42;
    for (int i = 0; i < 6; ++i)
        eye.emplace_back(shape.part(start + i).x(), shape.part(start + i).y());
    return eye;
}

bool FatigueDetector::detect(const cv::Mat& frame, cv::Mat& output) {
    output = frame.clone();

    dlib::cv_image<dlib::bgr_pixel> cimg(frame);
    std::vector<dlib::rectangle> faces = face_detector(cimg);
    if (faces.empty()) return false;

    
    dlib::rectangle biggest;
    int maxArea = 0;
    for (const auto& face : faces) {
        int area = face.width() * face.height();
        if (area > maxArea) {
            maxArea = area;
            biggest = face;
        }
    }

    
    cv::rectangle(output,
                  cv::Point(biggest.left(), biggest.top()),
                  cv::Point(biggest.right(), biggest.bottom()),
                  cv::Scalar(255, 0, 0), 2);

    
    dlib::full_object_detection shape = predictor(cimg, biggest);
    auto left_eye = extract_eye(shape, true);
    auto right_eye = extract_eye(shape, false);

    
    float ear = (eye_aspect_ratio(left_eye) + eye_aspect_ratio(right_eye)) / 2.0f;

    
    for (const auto& pt : left_eye) cv::circle(output, pt, 2, cv::Scalar(0, 255, 0), -1);
    for (const auto& pt : right_eye) cv::circle(output, pt, 2, cv::Scalar(0, 255, 0), -1);

    
    cv::Point2f eye_center((left_eye[0].x + right_eye[3].x) / 2.0f,
                           (left_eye[0].y + right_eye[3].y) / 2.0f);
    cv::Mat_<float> measurement(2, 1);
    measurement(0) = eye_center.x;
    measurement(1) = eye_center.y;

    if (!kalmanInitialized) {
        KF.statePost.at<float>(0) = eye_center.x;
        KF.statePost.at<float>(1) = eye_center.y;
        KF.statePost.at<float>(2) = 0;
        KF.statePost.at<float>(3) = 0;
        kalmanInitialized = true;
    }

    KF.predict();
    KF.correct(measurement);

  
    auto now = std::chrono::high_resolution_clock::now();
    double closedTime = 0.0;

    if (ear < EAR_THRESHOLD) {
        if (!eyeClosed) {
            lastBlinkStart = now;
            eyeClosed = true;
        } else {
            closedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlinkStart).count() / 1000.0;
        }
    } else {
        eyeClosed = false;
        closedTime = 0.0;
    }

    std::string status;
    if (ear >= EAR_THRESHOLD) {
        status = "NORMAL";
    } else if (closedTime < 1.5) {
        status = "BLINKING";
    } else {
        status = "FATIGUE";
    }

    
    cv::putText(output, "EAR: " + std::to_string(ear), cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
    cv::putText(output, "State: " + status, cv::Point(10, 60),
                cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 255), 2);

    if (status == "FATIGUE") {
        cv::putText(output, "DROWSINESS ALERT!", cv::Point(50, 100),
                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
        return true;
    }

    return false;
}