#include "MPU_kalman.h"
//
void Kalman::initKalmanFilter(KalmanFilter &kf)
{
    kf.angle = 0.0f; //Initialize states
    kf.bias = 0.0f;
    kf.P[0][0] = 0.0f;//Initialize error covariance matrix
    kf.P[0][1] = 0.0f;
    kf.P[1][0] = 0.0f;
    kf.P[1][1] = 0.0f;
    kf.Q_angle = 0.001f;//Set process noise covariance
    kf.Q_bias = 0.003f;
    kf.R_measure = 0.03f;
}

float Kalman::kalmanUpdate(KalmanFilter &kf, float newRate, float dt, float measuredAngle)
{
    kf.angle += dt * (newRate - kf.bias);
    
    // Updating the error covariance matrix
    kf.P[0][0] += dt * (dt * kf.P[1][1] - kf.P[0][1] - kf.P[1][0] + kf.Q_angle);
    kf.P[0][1] -= dt * kf.P[1][1];
    kf.P[1][0] -= dt * kf.P[1][1];
    kf.P[1][1] += kf.Q_bias * dt;

    //Calculate Kalman gain
    float S = kf.P[0][0] + kf.R_measure;
    float K0 = kf.P[0][0] / S;
    float K1 = kf.P[1][0] / S;
    // Calculate residual and update state estimate    
    float y = measuredAngle - kf.angle;
    kf.angle += K0 * y;
    kf.bias += K1 * y;

    float P00_temp = kf.P[0][0];
    float P01_temp = kf.P[0][1];
    // Calculate Kalman gain
    // P = F * P * F^T + Q
    kf.P[0][0] -= K0 * P00_temp;
    kf.P[0][1] -= K0 * P01_temp;
    kf.P[1][0] -= K1 * P00_temp;
    kf.P[1][1] -= K1 * P01_temp;//Temp storage

    return kf.angle;//Return optimized angle
}

