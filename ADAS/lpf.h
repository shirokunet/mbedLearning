#ifndef LPF_h
#define LPF_h

#include "mbed.h"

class LPF
{
    public:
        LPF(double gain = 0.1)
        {
            _gain = gain;
            _val = 0.0;
        }

        void setval(double);
        void setgain(double);
        double pass(double);
    private:
        double _val;
        double _gain;    
};

#endif /* LPF_h */
