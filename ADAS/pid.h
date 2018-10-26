#ifndef PID_h
#define PID_h

#include "mbed.h"

class PID
{
    public:
        PID(float pgain, float igain, float dgain, float max = 1.0)
        {
            _max = max;
            _pgain = pgain;
            _igain = igain;
            _dgain = dgain;
            _proportional = 0.0;
            _integral = 0.0;
            _derivative = 0.0;
            _error_z1 = 0.0;
            _error_z2 = 0.0;
            _manipulate_z1 = 0.0;
            _cycle_hz = 1000;
        }

        float calcPID(float error, bool reset = false);
        float calcSpeedPID(float error, bool reset = false);
        float get_proportional();
        float get_integral();
        float get_derivative();

    private:
        int _cycle_hz;
        float _max;
        float _pgain;
        float _igain;
        float _dgain;
        volatile float _proportional;
        volatile float _integral;
        volatile float _derivative;
        float _error_z1;
        float _error_z2;
        float _manipulate_z1;
};

#endif /* PID_h */
