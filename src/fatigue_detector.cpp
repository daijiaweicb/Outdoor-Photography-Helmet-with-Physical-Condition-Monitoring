#include "fatigue_detector.h"

/**
 * @brief Loads the dlib and OpenCV models from disk.
 */
FatigueDetector::FatigueDetector(const std::string &model_dir)
{
    dlib::deserialize(model_dir + "shape_predictor_68_face_landmarks.dat") >> predictor;

    face_net = cv::dnn::readNetFromCaffe(
        model_dir + "deploy.prototxt",
        model_dir + "res10_300x300_ssd_iter_140000.caffemodel");
    face_net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
    face_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
}

/**
 * @brief Computes EAR using the Euclidean distance between vertical and horizontal eye landmarks.
 */
float FatigueDetector::eye_aspect_ratio(const std::vector<cv::Point2f> &eye)
{
    float A = cv::norm(eye[1] - eye[5]);
    float B = cv::norm(eye[2] - eye[4]);
    float C = cv::norm(eye[0] - eye[3]);
    return (A + B) / (2.0f * C);
}

/**
 * @brief Extracts 6 eye landmarks from dlib's 68-point shape.
 */
std::vector<cv::Point2f> FatigueDetector::extract_eye(const dlib::full_object_detection &shape, bool left)
{
    std::vector<cv::Point2f> eye;
    int start = left ? 36 : 42;
    for (int i = 0; i < 6; ++i)
        eye.emplace_back(shape.part(start + i).x(), shape.part(start + i).y());
    return eye;
}

/**
 * @brief Main detection function that performs:
 *  - Face detection (every 5 frames)
 *  - Landmark extraction
 *  - Eye smoothing
 *  - EAR computation and fatigue logic
 */
bool FatigueDetector::detect(const cv::Mat &frame, cv::Mat &output)
{
    output = frame.clone();
    int w = frame.cols, h = frame.rows;

    // Detect face every 5 frames or if face was lost
    if (frame_count % 5 == 0 || !has_face)
    {
        has_face = false;
        cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300),
                                              cv::Scalar(104.0, 177.0, 123.0), false, false);
        face_net.setInput(blob);
        cv::Mat detections = face_net.forward();

        float *data = (float *)detections.ptr<float>(0, 0);
        int num = detections.size[2];

        for (int i = 0; i < num; ++i)
        {
            float conf = data[i * 7 + 2];
            if (conf < 0.5f)
                continue;

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

    if (!has_face)
        return false;

    // Draw face bounding box
    cv::rectangle(output, last_face, cv::Scalar(255, 0, 0), 2);

    // Run facial landmark detection
    dlib::cv_image<dlib::bgr_pixel> cimg(frame);
    dlib::rectangle dlib_rect(last_face.x, last_face.y,
                              last_face.x + last_face.width,
                              last_face.y + last_face.height);
    dlib::full_object_detection shape = predictor(cimg, dlib_rect);

    // Extract eye landmarks
    auto left_eye_raw = extract_eye(shape, true);
    auto right_eye_raw = extract_eye(shape, false);

    // Initialize smoothing
    if (!landmark_initialized)
    {
        prev_left_eye = left_eye_raw;
        prev_right_eye = right_eye_raw;
        landmark_initialized = true;
    }

    // Initialize smoothing
    std::vector<cv::Point2f> left_eye(6), right_eye(6);
    for (int i = 0; i < 6; ++i)
    {
        left_eye[i] = 0.7f * prev_left_eye[i] + 0.3f * left_eye_raw[i];
        right_eye[i] = 0.7f * prev_right_eye[i] + 0.3f * right_eye_raw[i];
    }
    prev_left_eye = left_eye;
    prev_right_eye = right_eye;

    // Draw eye landmarks
    for (const auto &pt : left_eye)
        cv::circle(output, pt, 2, cv::Scalar(0, 255, 0), -1);
    for (const auto &pt : right_eye)
        cv::circle(output, pt, 2, cv::Scalar(0, 255, 0), -1);

    // Compute EAR
    float ear = (eye_aspect_ratio(left_eye) + eye_aspect_ratio(right_eye)) / 2.0f;

    cv::putText(output, "EAR: " + std::to_string(ear), cv::Point(20, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

    // Drowsiness logic
    if (ear < EAR_THRESHOLD)
    {
        counter++;
        if (counter >= EYES_CLOSED_FRAMES)
        {
            cv::putText(output, "DROWSINESS ALERT!", cv::Point(50, 50),
                        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
            return true;
        }
    }
    else
    {
        counter = 0;
    }

    return false;
}
