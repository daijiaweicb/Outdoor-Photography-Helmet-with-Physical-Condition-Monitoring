#ifndef FATIGUE_DETECTOR_H
#define FATIGUE_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>

/**
 * @class FatigueDetector
 * @brief Performs real-time driver fatigue detection using facial landmarks and eye aspect ratio (EAR).
 *
 * This class combines OpenCV's DNN module for face detection and dlib's shape predictor
 * for facial landmark detection. Fatigue is inferred from the eye aspect ratio (EAR)
 * over a sequence of frames.
 */
class FatigueDetector
{
public:
    /**
     * @brief Constructs the detector and loads model files from the specified directory.
     * @param model_dir Path to the directory containing the following model files:
     *  - shape_predictor_68_face_landmarks.dat (dlib landmark predictor)
     *  - deploy.prototxt (Caffe model architecture)
     *  - res10_300x300_ssd_iter_140000.caffemodel (Caffe face detector weights)
     */
    FatigueDetector(const std::string &model_dir = "models/");

    /**
     * @brief Detects fatigue from a given frame.
     * @param frame Input BGR image.
     * @param output Copy of input with annotations (face box, eyes, EAR value).
     * @return true if drowsiness is detected, false otherwise.
     */
    bool detect(const cv::Mat &frame, cv::Mat &output);

private:
    dlib::shape_predictor predictor; // Facial landmark predictor (68 points)
    cv::dnn::Net face_net;           // OpenCV face detection network

    const float EAR_THRESHOLD = 0.20f; /// EAR below this indicates eyes likely closed
    const int EYES_CLOSED_FRAMES = 15; // Number of consecutive frames with low EAR to trigger alert
    int counter = 0;                   // Counts consecutive low-EAR frames

    int frame_count = 0;   // Used to control frequency of face detection
    bool has_face = false; // Whether a face was detected in the previous frame
    cv::Rect last_face;    //  Bounding box of the last detected face

    bool landmark_initialized = false;       ///< Whether landmark smoothing has been initialized
    std::vector<cv::Point2f> prev_left_eye;  ///< Previous frame's left eye landmarks (for smoothing)
    std::vector<cv::Point2f> prev_right_eye; ///< Previous frame's right eye landmarks

    /**
     * @brief Computes the eye aspect ratio (EAR) from a set of 6 eye landmarks.
     */
    float eye_aspect_ratio(const std::vector<cv::Point2f> &eye);

    /**
     * @brief Extracts 6 eye landmarks (left or right) from dlib's full shape.
     */
    std::vector<cv::Point2f> extract_eye(const dlib::full_object_detection &shape, bool left);
};

#endif // FATIGUE_DETECTOR_H
