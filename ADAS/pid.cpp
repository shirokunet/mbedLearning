#include "pid.h"

float PID::calcPID (float error, bool reset)
{
    float manipulate = 0.0;

    /* reset */
    if (reset){
        _integral = 0;
        _error_z1 = 0;
    }

    /* P */
    _proportional = error * _pgain;

    /* I */
    _integral += error * _igain;
    if (_integral > _max*0.4)
        _integral = _max*0.4;
    else if (_integral < -_max*0.4)
        _integral = -_max*0.4;

    /* D */
    _derivative = (error - _error_z1) * _dgain;

    /* control variable */
    manipulate = _proportional + _integral + _derivative;

    /* limit */
    if(manipulate > _max)
        manipulate = _max;
    else if(manipulate < -_max)
        manipulate = -_max;

    /* z1 */
    _error_z1 = error;

    return manipulate;
}

float PID::calcSpeedPID (float error, bool reset)
{
    float d_manipulate = 0.0;
    float manipulate = 0.0;

    /* reset */
    if (reset){
        _integral = 0;
        _error_z1 = 0;
    }

    /* P */
    _proportional = (error - _error_z1) / float(_cycle_hz)  * _pgain;

    /* I */
    _integral = error * _igain;

    /* D */
    _derivative = ((error - _error_z1) / float(_cycle_hz) -  (_error_z1 - _error_z2) / float(_cycle_hz))  / float(_cycle_hz) * _dgain;

    /* control variable */
    d_manipulate = _proportional + _integral + _derivative;
    manipulate = _manipulate_z1 + d_manipulate;

    /* limit */
    if(manipulate > _max)
        manipulate = _max;
    else if(manipulate < -_max)
        manipulate = -_max;

    /* z1 */
    _error_z2 = _error_z1;
    _error_z1 = error;
    _manipulate_z1 = manipulate;

    return manipulate;
}

float PID::get_proportional()
{
    return _proportional;
}

float PID::get_integral()
{
    return _integral;
}

float PID::get_derivative()
{
    return _derivative;
}