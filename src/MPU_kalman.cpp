#include "MPU_kalman.h"
//Kalman filter initialization and update
void Kalman::initKalmanFilter(KalmanFilter &kf)
{
    kf.angle = 0.0f;
    kf.bias = 0.0f;
    kf.P[0][0] = 0.0f;
    kf.P[0][1] = 0.0f;
    kf.P[1][0] = 0.0f;
    kf.P[1][1] = 0.0f;
    kf.Q_angle = 0.001f;
    kf.Q_bias = 0.003f;
    kf.R_measure = 0.03f;
}
// Kalman filter update function
float Kalman::kalmanUpdate(KalmanFilter &kf, float newRate, float dt, float measuredAngle)
{
    // Prediction phase: Update state using gyro data
    kf.angle += dt * (newRate - kf.bias);

    // Updating the error covariance matrix
    kf.P[0][0] += dt * (dt * kf.P[1][1] - kf.P[0][1] - kf.P[1][0] + kf.Q_angle);
    kf.P[0][1] -= dt * kf.P[1][1];
    kf.P[1][0] -= dt * kf.P[1][1];
    kf.P[1][1] += kf.Q_bias * dt;

    // Measurement update phase
    float S = kf.P[0][0] + kf.R_measure;
    float K0 = kf.P[0][0] / S;
    float K1 = kf.P[1][0] / S;

    float y = measuredAngle - kf.angle;
    kf.angle += K0 * y;
    kf.bias += K1 * y;
    
    // Update error covariance matrix
    float P00_temp = kf.P[0][0];
    float P01_temp = kf.P[0][1];
    kf.P[0][0] -= K0 * P00_temp;
    kf.P[0][1] -= K0 * P01_temp;
    kf.P[1][0] -= K1 * P00_temp;
    kf.P[1][1] -= K1 * P01_temp;
    //Return optimized angle
    return kf.angle;
}

