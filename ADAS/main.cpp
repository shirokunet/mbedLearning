#include "mbed.h"

#include <stdio.h>
#include <string>

#include "Kalman.h"
#include "mc33926.h"
#include "encoder.h"
#include "hx711.h"
#include "lpf.h"
#include "pid.h"
#include "serial.h"


AnalogIn potentio_a(A2);
AnalogIn potentio_t(A5);
DigitalIn button_y(PA_13, PullUp);
DigitalIn button_g(PA_14, PullUp);
DigitalIn button_r(PA_15, PullUp);
LPF lpf_potentio_t(0.2);
MC33926 md(D7, D9, A0, D8, D10, A1, D4, D12);
PID pidPos(0.1, 0.001, 0.1);
PID pidVel(0.01, 0.0001, 0.01);
PID pidCur(0.01, 0.0001, 0.01);
HallEncoder encoder(A3, A4);
DigitalOut led(LED3);
Kalman kalman;


Timer tm_main;
Ticker tk_update;
Ticker tk_serial;

#define ANGLE_OFFSET -7.0
#define TARGET_VELOCITY_LIMIT 50.0
#define TARGET_CURRENT_LIMIT 2500.0


// #define SERIAL_PC
#define SERIAL_CAM

#if defined SERIAL_PC
    Serial pc(USBTX, USBRX, 115200);
#elif defined SERIAL_CAM
    int _data_tx[_data_tx_size] = {0};
    int _data_rx[_data_rx_size] = {0};
    RawSerial pc(USBTX, USBRX, 115200);
    SerialTW stw(&pc);
#endif


volatile bool _flagControl = false;
volatile bool _flagSerialTx = false;

void IRQ_Control()
{
    _flagControl = true;
}

void IRQ_SerialTx()
{
    _flagSerialTx = true;
}

void IRQ_SerialRx()
{
    led = !led;
    char ch = pc.getc();
#if defined SERIAL_CAM
    stw.read(ch, _data_rx);
#endif
}

int main() 
{
    tm_main.reset();
    tm_main.start();

    /* interrupt setting */
    const float control_cycle_sec = 0.001;
    const float serialtx_cycle_sec = 0.016;
    tk_update.attach(&IRQ_Control, control_cycle_sec);
    tk_serial.attach(&IRQ_SerialTx, serialtx_cycle_sec);
#if defined SERIAL_CAM
    pc.attach(&IRQ_SerialRx, Serial::RxIrq);
#endif

    /* store variable */
    int time_us_diff = 0;
    int time_us_z1 = 0;
    int counter_servo_unit = 0;
    double target_angle_z1 = 0.0;
    double actual_angle = 0.0;

    while(1)
    { 
#if defined SERIAL_CAM
        if (_flagSerialTx)
        {
            _flagSerialTx = false;
            _data_tx[0] = counter_servo_unit;
            _data_tx[1] = time_us_diff;
            _data_tx[2] = int(actual_angle*1000.0);  // LSB:0.001 [deg]
            _data_tx[3] = int(!button_y); 
            _data_tx[4] = int(!button_g);
            _data_tx[5] = int(!button_r);
            stw.write(_data_tx);
        }
#endif
        if (_flagControl)
        {
            _flagControl = false;

            /* measure time */
            int time_us = tm_main.read_us();
            time_us_diff = time_us - time_us_z1;

            /* serial rx update */
#if defined SERIAL_CAM
            int counter_camera_unit = _data_rx[0];
            int servo_on_flag = _data_rx[1];
            double target_angle = double(_data_rx[2])/1000.0;   // LSB:0.001 [deg]
#endif
            /* sensors */
            double actual_angle_raw = double(potentio_a.read_u16() - 32768) / 65536.0 * (360.0 * 4.0) - ANGLE_OFFSET;
            double actual_velocity = encoder.getDPS_lpf() * 0.1395;             // pulley ratio 12:86 
            actual_angle = kalman.getAngle(actual_angle_raw, actual_velocity, control_cycle_sec);

            /* position FB, unit:deg */
#if defined SERIAL_PC
            double target_angle = lpf_potentio_t.pass(double(potentio_t.read_u16() - 32768) / 65536.0 * 180.0);
#endif
            double error_angle = target_angle - actual_angle;
            double u_angle = pidPos.calcPID(error_angle);

            /* velocity FB, unit:deg/sec */
            double target_velocity_ff = (target_angle - target_angle_z1)/control_cycle_sec * 0.5;
            double target_velocity_fb = u_angle * 200.0;
            double target_velocity = target_velocity_ff + target_velocity_fb;
            if (target_velocity > TARGET_VELOCITY_LIMIT)
                target_velocity = TARGET_VELOCITY_LIMIT;
            else if (target_velocity < -TARGET_VELOCITY_LIMIT)
                target_velocity = -TARGET_VELOCITY_LIMIT;
            double error_velocity = target_velocity - actual_velocity;
            double u_velocity = pidVel.calcSpeedPID(error_velocity);

            /* current FB, unit:mili ampare  */
            double target_current_ff = target_velocity / 250.0 * 400.0;
            // double target_current_fb = u_velocity * 1200.0;
            double target_current_fb = (u_velocity * 0.4 + u_angle * 0.6) * TARGET_CURRENT_LIMIT;
            double target_current = target_current_ff + target_current_fb;
            double actual_current = md.getM1CurrentMilliamps();
            if (target_current > TARGET_CURRENT_LIMIT)
                target_current = TARGET_CURRENT_LIMIT;
            else if (target_current < -TARGET_CURRENT_LIMIT)
                target_current = -TARGET_CURRENT_LIMIT;
            double error_current = target_current - actual_current;
            double u_current = pidCur.calcSpeedPID(error_current);

            /* control motor */
#if defined SERIAL_CAM
            if (servo_on_flag)
                md.setM1Speed(u_current);
            else
                md.setM1Speed(0.0);
#else
            md.setM1Speed(u_current);
#endif
            /* z1 */
            time_us_z1 = time_us;
            target_angle_z1 = target_angle;

            counter_servo_unit++;

#if defined SERIAL_PC
            if (_flagSerialTx)
            {
                _flagSerialTx = false;

                pc.printf("t_a:%3.1f,", target_angle);
                pc.printf("a_a:%f,", actual_angle);
                pc.printf("p:%3.3f,", pidPos.get_proportional()* 100.0);
                pc.printf("i:%3.3f,", pidPos.get_integral()* 100.0);
                pc.printf("d:%3.3f,", pidPos.get_derivative()* 100.0);
                pc.printf("\t");

                pc.printf("t_v_ff:%3.3f,", target_velocity_ff);
                pc.printf("t_v_fb:%3.3f,", target_velocity_fb);
                pc.printf("t_v:%3.3f,", target_velocity);
                pc.printf("a_v:%3.3f,", actual_velocity);
                // pc.printf("p: %3.3f,", pidVel.get_proportional() * 100.0);
                // pc.printf("i: %3.3f,", pidVel.get_integral() * 100.0);
                // pc.printf("d: %3.3f,", pidVel.get_derivative() * 100.0);
                pc.printf("\t");

                pc.printf("t_c_ff:%3.3f,", target_current_ff);
                pc.printf("t_c_fb:%3.3f,", target_current_fb);
                pc.printf("t_c:%3.3f,", target_current);
                pc.printf("a_c:%3.3f,", actual_current);
                // pc.printf("p: %3.3f,", pidCur.get_proportional() * 100.0);
                // pc.printf("i: %3.3f,", pidCur.get_integral() * 100.0);
                // pc.printf("d: %3.3f,", pidCur.get_derivative() * 100.0);
                pc.printf("\t");

                pc.printf("u_c:%3.3f,", u_current);
                // pc.printf("rpm: %3.3f,", encoder.getRPM_lpf());
                
                // pc.printf("pos: %d,", encoder.getPosition());
                pc.printf("ms:%d", int(time_us_diff/1000.0f));
                pc.printf("\r\n");
            }
#endif
        }
    }
}
