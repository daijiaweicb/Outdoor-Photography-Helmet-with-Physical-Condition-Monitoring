#ifndef MPU_KALMAN_H
#define MPU_KALMAN_H

class Kalman
{
public:
    struct KalmanFilter
    {
        float angle;     // Estimated perspective
        float bias;      // Estimated gyro zero bias
        float P[2][2];   // Error covariance matrix
        float Q_angle;   // Process noise variance (angle)
        float Q_bias;    // Process noise variance (zero bias)
        float R_measure; // Observation noise variance
    };

    void initKalmanFilter(KalmanFilter &kf);
    float kalmanUpdate(KalmanFilter &kf, float newRate, float dt, float measuredAngle);
};

#endif