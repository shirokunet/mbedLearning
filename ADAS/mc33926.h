#ifndef MC33926_h
#define MC33926_h

#include "mbed.h"

class MC33926
{
    public:
        MC33926(PinName M1DIR, PinName M1PWM, PinName M1FB, 
                               PinName M2DIR, PinName M2PWM, PinName M2FB, 
                               PinName nD2, PinName nSF, float lpf_gain = 0.08, int pwm_period_us = 50) : 
                               _M1DIR(M1DIR), _M1PWM(M1PWM), _M1FB(M1FB), 
                               _M2DIR(M2DIR), _M2PWM(M2PWM), _M2FB(M2FB), 
                               _nD2(nD2), _nSF(nSF)
        {
            _nD2 = 1;
            _lpf_gain = lpf_gain;
            _dir = -1;

            /* PWM operation up to 20 kHz, which is ultrasonic and allows for quieter motor operation (20kHz = 50us) */
            _M1PWM.period_us(pwm_period_us);
            _M2PWM.period_us(pwm_period_us);

            /* set offset */
            _m1fb_offset = readAverageM1FB(30);
            _m2fb_offset = readAverageM2FB(30);
        }

        void setM1Speed(float speed);                 // Set speed for M1.
        void setM2Speed(float speed);                 // Set speed for M2.
        void setSpeeds(float m1Speed, float m2Speed); // Set speed for both M1 and M2.
        int readAverageM1FB(int times);               // Get average of current for M1
        int readAverageM2FB(int times);               // Get average of current for M2
        float getM1CurrentMilliamps();                // Get current reading for M1.
        float getM2CurrentMilliamps();                // Get current reading for M2.
        unsigned char getFault();                     // Get fault reading.

    private:
        DigitalOut _M1DIR;
        PwmOut     _M1PWM;
        AnalogIn   _M1FB;
        DigitalOut _M2DIR;
        PwmOut     _M2PWM;
        AnalogIn   _M2FB;
        DigitalOut _nD2;
        DigitalIn  _nSF;

        int _m1fb_offset;
        int _m2fb_offset;
        int _m1dir;
        int _m2dir;
        float _lpf_gain;
        float _m1fb_lpf;
        float _m2fb_lpf;
        int _dir;
};

#endif /* MC33926_h */
