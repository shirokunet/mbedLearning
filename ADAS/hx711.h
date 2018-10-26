#ifndef HX711_h
#define HX711_h

#include "mbed.h"

class HX711
{
    public:
        // define clock and data pin, channel, and gain factor
        // channel selection is made by passing the appropriate gain: 128 or 64 for channel A, 32 for channel B
        // gain: 128 or 64 for channel A; channel B works with 32 gain factor only
        HX711(PinName pin_sck, PinName pin_dt, uint8_t gain = 128) : 
            PD_SCK(pin_sck, 0), DATA(pin_dt), _thread(osPriorityNormal)
        {
            set_offset(0);
            set_scale(1.0f);
            set_gain(gain);
            tare();
            thread_start();
        }

        // set the gain factor; takes effect only after a call to read()
        // channel A can be set for a 128 or 64 gain; channel B has a fixed 32 gain
        // depending on the parameter, the channel is also set to either A or B
        void set_gain(int gain = 128);

        // waits for the chip to be ready and returns a reading
        uint32_t readRaw();
        long read();

        // returns an average reading; times = how many times to read
        long read_average(int times = 10);

        // returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
        double get_value(int times = 1);

        // returns get_value() divided by SCALE, that is the raw value divided by a value obtained via calibration
        // times = how many readings to do
        float get_units(int times = 1);

        // set the OFFSET value for tare weight; times = how many times to read the tare value
        void tare(int times = 10);

        // set the SCALE value; this value is used to convert the raw data to "human readable" data (measure units)
        void set_scale(float scale = 1.f);

        // get the current SCALE
        float get_scale();

        // set OFFSET, the value that's subtracted from the actual reading (tare weight)
        void set_offset(long offset = 0);

        // get the current OFFSET
        long get_offset();

        // Thread
        void thread_start() {
            _thread.start(callback(this, &HX711::threadLoop));
        }

        long get_thvalue() {
            return _value;
        }

    private:
        DigitalOut PD_SCK;  // Power Down and Serial Clock Input Pin
        DigitalIn DATA;     // Serial Data Output Pin
        int GAIN;           // amplification factor
        long OFFSET;        // used for tare weight
        float SCALE;        // used to return weight in grams, kg, ounces, whatever
        Thread _thread;
        volatile long _value;

        // check if HX711 is ready
        // from the datasheet: When output data is not ready for retrieval, digital output pin DATA is high. Serial clock
        // input PD_SCK should be low. When DATA goes to low, it indicates data is ready for retrieval.
        bool is_ready();

        // puts the chip into power down mode
        void power_down();

        // wakes up the chip after power down mode
        void power_up();

        void threadLoop() {
            while(1) {
                _value = read();
            }
        }
};

#endif /* HX711_h */
