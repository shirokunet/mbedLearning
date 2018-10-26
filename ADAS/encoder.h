#ifndef HallEncoder_h
#define HallEncoder_h

#include "mbed.h"

class HallEncoder
{
    public:
        HallEncoder(PinName pin_A, PinName pin_B, unsigned int time_us = 5000)
                    : _encoder_A(pin_A), _encoder_B(pin_B)
        {
            set_sample_time(time_us);
            set_cnt();
            _encoder_A.rise(callback(this, &HallEncoder::ISR_A));
            _encoder_B.rise(callback(this, &HallEncoder::ISR_B));
            _tkRPM.attach_us(callback(this, &HallEncoder::calcRPM), _SampleTime_us);
            _pulses = 0;
            _prev_cnt = 0;
            _cur_cnt = 0;
            _rpm = 0.0;
            _rpm_lpf = 0.0;
            _dps_lpf = 0.0;
            _lpf_gain = 0.1;
            _dir = -1;
            _gear_ratio = 5.2;
        }

        float getRPM(void);
        float getRPM_lpf(void);
        float getDPS(void);
        float getDPS_lpf(void);
        long getPosition(void);

    private:
        InterruptIn _encoder_A;
        InterruptIn _encoder_B;
        Ticker _tkRPM;

        unsigned int _EncoderCntsperRev;
        unsigned int _SampleTime_us;
        volatile float _rpm;
        volatile float _rpm_lpf;
        volatile float _dps_lpf;
        volatile long _pulses;
        int _dir;
        long _prev_cnt, _cur_cnt;
        float _lpf_gain;
        float _gear_ratio;

        void set_cnt(unsigned int cnt = 40);
        void set_sample_time(unsigned int time_us);
        void ISR_A(void);
        void ISR_B(void);
        void calcRPM(void);
};

#endif /* HallEncoder_h */
