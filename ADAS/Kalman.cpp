/* Copyright (C) 2012 Kristian Lauszus, TKJ Electronics. All rights reserved.

 This software may be distributed and modified under the terms of the GNU
 General Public License version 2 (GPL2) as published by the Free Software
 Foundation and appearing in the file GPL2.TXT included in the packaging of
 this file. Please note that GPL2 Section 2[b] requires that all works based
 on this software must also be made publicly available under the terms of
 the GPL2 ("Copyleft").

 Contact information
 -------------------

 Kristian Lauszus, TKJ Electronics
 Web      :  http://www.tkjelectronics.com
 e-mail   :  kristianl@tkjelectronics.com
 */

#include "Kalman.h"

Kalman::Kalman() {
    /* We will set the variables like so, these can also be tuned by the user */
    Q_angle = 0.007;
    Q_bias = 0.001;
    R_measure = 0.03;

    angle = 0.0; // Reset the angle
    bias = 0.0; // Reset bias

    P[0][0] = 0.0; // Since we assume that the bias is 0 and we know the starting angle (use setAngle), the error covariance matrix is set like so - see: http://en.wikipedia.org/wiki/Kalman_filter#Example_application.2C_technical
    P[0][1] = 0.0;
    P[1][0] = 0.0;
    P[1][1] = 0.0;

    _actual_angle_lim = 90.0;
};

// The angle should be in degrees and the rate should be in degrees per second and the delta time in seconds
double Kalman::getAngle(double newAngle, double newRate, double dt) {
    // KasBot V2  -  Kalman filter module - http://www.x-firm.com/?page_id=145
    // Modified by Kristian Lauszus
    // See my blog post for more information: http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it

    // Discrete Kalman filter time update equations - Time Update ("Predict")
    // Update xhat - Project the state ahead
    /* Step 1 */
    rate = newRate - bias;
    angle += dt * rate;

    // Update estimation error covariance - Project the error covariance ahead
    /* Step 2 */
    P[0][0] += dt * (dt*P[1][1] - P[0][1] - P[1][0] + Q_angle);
    P[0][1] -= dt * P[1][1];
    P[1][0] -= dt * P[1][1];
    P[1][1] += Q_bias * dt;

    // Discrete Kalman filter measurement update equations - Measurement Update ("Correct")
    // Calculate Kalman gain - Compute the Kalman gain
    /* Step 4 */
    double S = P[0][0] + R_measure; // Estimate error
    /* Step 5 */
    double K[2]; // Kalman gain - This is a 2x1 vector
    K[0] = P[0][0] / S;
    K[1] = P[1][0] / S;

    // Calculate angle and bias - Update estimate with measurement zk (newAngle)
    /* Step 3 */
    double y = newAngle - angle; // Angle difference
    /* Step 6 */
    angle += K[0] * y;
    bias += K[1] * y;

    // Calculate estimation error covariance - Update the error covariance
    /* Step 7 */
    double P00_temp = P[0][0];
    double P01_temp = P[0][1];

    P[0][0] -= K[0] * P00_temp;
    P[0][1] -= K[0] * P01_temp;
    P[1][0] -= K[1] * P00_temp;
    P[1][1] -= K[1] * P01_temp;

    return angle;
};

double Kalman::getAngleLim(double newAngle, double newRate, double dt) {
    double angle = getAngle(newAngle, newRate, dt);

    if (angle > _actual_angle_lim)
        angle = _actual_angle_lim;
    else if (angle < -_actual_angle_lim)
        angle = -_actual_angle_lim;

    return angle;
}

void Kalman::setAngle(double angle) { this->angle = angle; }; // Used to set angle, this should be set as the starting angle
double Kalman::getRate() { return this->rate; }; // Return the unbiased rate

/* These are used to tune the Kalman filter */
void Kalman::setQangle(double Q_angle) { this->Q_angle = Q_angle; };
void Kalman::setQbias(double Q_bias) { this->Q_bias = Q_bias; };
void Kalman::setRmeasure(double R_measure) { this->R_measure = R_measure; };

double Kalman::getQangle() { return this->Q_angle; };
double Kalman::getQbias() { return this->Q_bias; };
double Kalman::getRmeasure() { return this->R_measure; };
