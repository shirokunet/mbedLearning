#ifndef SERIALTW_h
#define SERIALTW_h

#include "mbed.h"

#define _data_tx_size 6
#define _data_rx_size 3

class SerialTW
{
    public:
        SerialTW(RawSerial *pc) : _pc(pc){
        }
        bool read(char ch, int data_rx[]);
        void write(int data_tx[]);
    private:
        RawSerial *_pc;
};

#endif /* SERIALTW_h */
