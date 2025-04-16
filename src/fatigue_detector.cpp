#include "fatigue_detector.h"

FatigueDetector::FatigueDetector() {
    
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> predictor;

   
    face_net = cv::dnn::readNetFromCaffe(
        "deploy.prototxt",
        "res10_300x300_ssd_iter_140000.caffemodel"
    );

    
    face_net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
    face_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
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
    int w = frame.cols;
    int h = frame.rows;

    
    cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300),
                                          cv::Scalar(104.0, 177.0, 123.0), false, false);
    face_net.setInput(blob);
    cv::Mat detections = face_net.forward();

    
    float* data = (float*)detections.ptr<float>(0, 0);
    int num_detections = detections.size[2];

    for (int i = 0; i < num_detections; ++i) {
        float confidence = data[i * 7 + 2];
        if (confidence < 0.5f) continue;

        int x1 = static_cast<int>(data[i * 7 + 3] * w);
        int y1 = static_cast<int>(data[i * 7 + 4] * h);
        int x2 = static_cast<int>(data[i * 7 + 5] * w);
        int y2 = static_cast<int>(data[i * 7 + 6] * h);

        cv::Rect face_box(x1, y1, x2 - x1, y2 - y1);
        cv::rectangle(output, face_box, cv::Scalar(255, 0, 0), 2);

        dlib::cv_image<dlib::bgr_pixel> cimg(frame);
        dlib::rectangle dlib_rect(x1, y1, x2, y2);
        dlib::full_object_detection shape = predictor(cimg, dlib_rect);

        auto left_eye = extract_eye(shape, true);
        auto right_eye = extract_eye(shape, false);
        float ear = (eye_aspect_ratio(left_eye) + eye_aspect_ratio(right_eye)) / 2.0f;

        for (const auto& pt : left_eye)  cv::circle(output, pt, 2, cv::Scalar(0, 255, 0), -1);
        for (const auto& pt : right_eye) cv::circle(output, pt, 2, cv::Scalar(0, 255, 0), -1);

        if (ear < EAR_THRESHOLD) {
            counter++;
            if (counter >= EYES_CLOSED_FRAMES) {
                cv::putText(output, "DROWSINESS ALERT!", cv::Point(50, 50),
                            cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
                return true;
            }
        } else {
            counter = 0;
        }
    }

    return false;
}
