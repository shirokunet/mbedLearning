#include "encoder.h"

void HallEncoder::set_cnt(unsigned int cnt) {
    _EncoderCntsperRev = cnt;
}

void HallEncoder::set_sample_time(unsigned int time_us) {
    _SampleTime_us = time_us;
}

void HallEncoder::ISR_A(void)
{
    // A:1, B:0
    if(_encoder_A.read() ^ _encoder_B.read())
    {
        _pulses += 1;
    }
    // A:1, B:1
    else
    {
         _pulses -= 1;
    }
}

void HallEncoder::ISR_B(void)
{
    // A:0, B:1
    if(_encoder_A.read() ^ _encoder_B.read())
    {
         _pulses -= 1;
    }
    // A:1, B:1
    else
    {
        _pulses += 1;
    }
}

long HallEncoder::getPosition() {
    return _dir * _pulses;
}

void HallEncoder::calcRPM(void)
{
    _cur_cnt = getPosition();
    _rpm = float(_cur_cnt-_prev_cnt)/float(_EncoderCntsperRev)/(float(_SampleTime_us)*1.e-6)*60.0 / _gear_ratio;
    _prev_cnt = _cur_cnt;
}

float HallEncoder::getRPM() {
    return _rpm;
}

float HallEncoder::getRPM_lpf() {
    _rpm_lpf = getRPM() * _lpf_gain + _rpm_lpf * (1.0 - _lpf_gain);
    return _rpm_lpf;
}

float HallEncoder::getDPS() {
    return _rpm / 60.0 * 360.0;
}

float HallEncoder::getDPS_lpf() {
    _dps_lpf = getDPS() * _lpf_gain + _dps_lpf * (1.0 - _lpf_gain);
    return _dps_lpf;
}
