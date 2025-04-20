#include "fatigue_detector.h"

// Real-time fatigue detection using eye aspect ratio
// Load face detection and landmark models
FatigueDetector::FatigueDetector() {
    // Load facial landmark predictor
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> predictor;
    // Load face detection model
    face_net = cv::dnn::readNetFromCaffe(
        "deploy.prototxt",
        "res10_300x300_ssd_iter_140000.caffemodel"
    );
    face_net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
    face_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
}

// Calculate Eye Aspect Ratio based on eye landmarks
float FatigueDetector::eye_aspect_ratio(const std::vector<cv::Point2f>& eye) {
    // Vertical distances A,B and horizontal distance C
    float A = cv::norm(eye[1] - eye[5]);
    float B = cv::norm(eye[2] - eye[4]);
    float C = cv::norm(eye[0] - eye[3]);
    return (A + B) / (2.0f * C);
}
// Extract eye landmarks from facial detection results
std::vector<cv::Point2f> FatigueDetector::extract_eye(const dlib::full_object_detection& shape, bool left) {
    std::vector<cv::Point2f> eye;
    int start = left ? 36 : 42; // Left eye: points 36-41, Right eye: 42-47
    for (int i = 0; i < 6; ++i)
        eye.emplace_back(shape.part(start + i).x(), shape.part(start + i).y());
    return eye;
}
// Main detection process with frame analysis
bool FatigueDetector::detect(const cv::Mat& frame, cv::Mat& output) {
    output = frame.clone();
    int w = frame.cols, h = frame.rows;

    // Face detection every 5 frames or when face lost
    if (frame_count % 5 == 0 || !has_face) {
        has_face = false;
        // Preprocess frame for face detection
        cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300),
                                              cv::Scalar(104.0, 177.0, 123.0), false, false);
        face_net.setInput(blob);
        cv::Mat detections = face_net.forward();

        float* data = (float*)detections.ptr<float>(0, 0);
        int num = detections.size[2];
        // Process detection results
        for (int i = 0; i < num; ++i) {
            float conf = data[i * 7 + 2];
            if (conf < 0.5f) continue;
            // Convert normalized coordinates to pixel values
            int x1 = static_cast<int>(data[i * 7 + 3] * w);
            int y1 = static_cast<int>(data[i * 7 + 4] * h);
            int x2 = static_cast<int>(data[i * 7 + 5] * w);
            int y2 = static_cast<int>(data[i * 7 + 6] * h);

            last_face = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
            has_face = true;
            break; 
        }
    }

    frame_count++;

    if (!has_face) return false;

    // Draw face bounding box
    cv::rectangle(output, last_face, cv::Scalar(255, 0, 0), 2);

    // Facial landmark detection using dlib
    dlib::cv_image<dlib::bgr_pixel> cimg(frame);
    dlib::rectangle dlib_rect(last_face.x, last_face.y,
                              last_face.x + last_face.width,
                              last_face.y + last_face.height);
    dlib::full_object_detection shape = predictor(cimg, dlib_rect);

    auto left_eye_raw = extract_eye(shape, true);
    auto right_eye_raw = extract_eye(shape, false);

    // Initialize landmark tracking
    if (!landmark_initialized) {
        prev_left_eye = left_eye_raw;
        prev_right_eye = right_eye_raw;
        landmark_initialized = true;
    }

    // Smooth eye coordinates using EMA filter
    std::vector<cv::Point2f> left_eye(6), right_eye(6);
    for (int i = 0; i < 6; ++i) {
        left_eye[i]  = 0.7f * prev_left_eye[i]  + 0.3f * left_eye_raw[i];
        right_eye[i] = 0.7f * prev_right_eye[i] + 0.3f * right_eye_raw[i];
    }
    prev_left_eye = left_eye;
    prev_right_eye = right_eye;

    // Visualize eye landmarks
    for (const auto& pt : left_eye)  cv::circle(output, pt, 2, cv::Scalar(0, 255, 0), -1);
    for (const auto& pt : right_eye) cv::circle(output, pt, 2, cv::Scalar(0, 255, 0), -1);
    // Calculate average EAR
    float ear = (eye_aspect_ratio(left_eye) + eye_aspect_ratio(right_eye)) / 2.0f;

    // Display EAR value
    cv::putText(output, "EAR: " + std::to_string(ear), cv::Point(20, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
    // Fatigue detection logic
    if (ear < EAR_THRESHOLD) {
        counter++;
        if (counter >= EYES_CLOSED_FRAMES) {
            cv::putText(output, "DROWSINESS ALERT!", cv::Point(50, 50),
                        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
            return true;
        }
    } else {
        counter = 0; // Reset consecutive frame counter
    }

    return false;
}
