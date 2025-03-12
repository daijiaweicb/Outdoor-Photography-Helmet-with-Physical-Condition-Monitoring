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

    /**
     * @brief init the kalman filter
     * @param  {KalmanFilter} kf : 
     */
    void initKalmanFilter(KalmanFilter &kf);

    
    /**
     * @brief  update the data
     * @param  {KalmanFilter} kf     : 
     * @param  {float} newRate       : 
     * @param  {float} dt            : 
     * @param  {float} measuredAngle : 
     * @return {float}               : 
     */
    float kalmanUpdate(KalmanFilter &kf, float newRate, float dt, float measuredAngle);
};

#endif