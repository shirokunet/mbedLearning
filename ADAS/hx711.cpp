#include "hx711.h"


bool HX711::is_ready() {
    return DATA == 0;
}

void HX711::set_gain(int gain) {
    switch (gain) {
        case 128:       // channel A, gain factor 128
            GAIN = 1;
            break;
        case 64:        // channel A, gain factor 64
            GAIN = 3;
            break;
        case 32:        // channel B, gain factor 32
            GAIN = 2;
            break;
    }

    PD_SCK = 0;
}

uint32_t HX711::readRaw() {
    uint32_t res = 0;

    while (!is_ready());

    for(int i = 0; i < 24; i++) {
        PD_SCK = 1;
        res = res << 1;
        PD_SCK = 0;
        if (DATA == 1) {
            res++;
        }
    }

    for (int i = 0; i < GAIN; i++) {
        PD_SCK = 1;
        PD_SCK = 0;
    }

    // res ^= 0x00800000;   // LPC1768
    res ^= 0x00400000;      // NUCLEO_F446RE

    return res;
}

long HX711::read() {
    return readRaw() - OFFSET;
}
long HX711::read_average(int times) {
    long sum = 0;
    for (int i = 0; i < times; i++) {
        sum += readRaw();
    }
    return sum / times;
}

double HX711::get_value(int times) {
    return read_average(times) - OFFSET;
}

float HX711::get_units(int times) {
    return get_value(times) / SCALE;
}

void HX711::tare(int times) {
    double sum = read_average(times);
    set_offset(sum);
}

void HX711::set_scale(float scale) {
    SCALE = scale;
}

float HX711::get_scale() {
    return SCALE;
}

void HX711::set_offset(long offset) {
    OFFSET = offset;
}

long HX711::get_offset() {
    return OFFSET;
}

void HX711::power_down() {
    PD_SCK = 0;
    PD_SCK = 1;
}

void HX711::power_up() {
    PD_SCK = 0;
}
