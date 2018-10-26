#include "serial.h"

bool SerialTW::read(char ch, int data_rx[])
{   
    static char _value[_data_rx_size][10];
    static int _index[10] = {0};
    static bool read_flag = false;
    static int num = -1;

    /* store */
    if (ch == '\r')
    {
        /* char to int */
        for (int i = 0; i < _data_rx_size; ++i)
        {
            _value[i][_index[i]]='\x0';   // add un 0 to end the c string 
            data_rx[i] = atoi(_value[i]);
        }

        /* reset */
        read_flag = false;
        num = -1;
        for (int i = 0; i < 10; ++i)
        {
            _index[i] = 0;
        }

        return true;
    }
    else if (ch == 's')
    {
        read_flag = true;
    }
    else if (ch == ',')
    {
        num ++;
    }
    else if (_index[num]<10-1 && read_flag)
    {
        _value[num][_index[num]++] = ch;
    }

    return false;
}

void SerialTW::write(int data_tx[])
{
    _pc->printf("s,");
    for (int i = 0; i < _data_tx_size; ++i)
    {
        _pc->printf("%d,", data_tx[i]);
    }
    _pc->printf("\r\n"); 
}