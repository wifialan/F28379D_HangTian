/*
 * fsk.c
 *
 *  Created on: 2018éªï¿½12éˆï¿½1é™ï¿½
 *      Author: multimicro
 */

#include "project.h"

extern AD9833 ad9833_dev, *ad9833_dev_handle;
#define _SHOW


void qfsk_send_data(Uint32 data)
{
    int i, tmp, data_len;
//    data_len = qfsk_data_send_len(data);
    data_len = 16;
    DELAY_MS(10);
#ifdef SHOW
    lfm(5*KHz,25*KHz,100*Hz);
#else
    lfm(20*KHz,26*KHz,30*Hz);
#endif
    DELAY_MS(8);//4ms delay between lfm and data
    for (i = 0; i < data_len; ++i) {
        tmp = data & 0x0003;//Send LSB first
        switch (tmp) {
#ifdef SHOW
        case 0:qfsk_freq_select(5ul*KHz);break;
        case 1:qfsk_freq_select(10ul*KHz);break;
        case 2:qfsk_freq_select(15ul*KHz);break;
        case 3:qfsk_freq_select(20ul*KHz);break;
#else
        case 0:qfsk_freq_select(20ul*KHz);break;
        case 1:qfsk_freq_select(22ul*KHz);break;
        case 2:qfsk_freq_select(24ul*KHz);break;
        case 3:qfsk_freq_select(26ul*KHz);break;
#endif
        default:break;
        }
        data >>= 2;
    }
    ad9833_dev_handle->reset(ad9833_dev_handle);//ÖØÖÃDDS
}

void qfsk_freq_select(Uint32 freq)
{
    ad9833_dev_handle->set_wave_freq(ad9833_dev_handle,freq,AD9833_REG_FREQ0);
    DELAY_MS(1.99);//ÑÓ³Ù2ms sin:1.99
    ad9833_dev_handle->reset(ad9833_dev_handle);//ÖØÖÃDDS
    DELAY_MS(1);//ÑÓ³Ù2ms sin:1.99
}


Uint16 qfsk_data_send_len(Uint32 data)
{
    int i, send_len;
    Uint32 tmp;
    send_len = 0;
    for (i = 0; i < 8; ++i) {
        tmp = data & 0xF0000000;
        if (tmp == 0)
            send_len += 1;
        data <<= 4;
    }
    send_len = ( 8 - send_len ) * 2;
    return send_len ;
}

void lfm(Uint16 start_freq, Uint16 end_freq, Uint16 freq_step)
{
    //20K ~ 26KÆµÂÊ·¶Î§req_step is 30Hz, should change 6k/30 = 200 times
    Uint16 bandwidth,i,bandwidth_step,current_freq;
    bandwidth = end_freq - start_freq;
    bandwidth_step = bandwidth / freq_step;
    current_freq = start_freq;
    for (i = 0; i < bandwidth_step; ++i) {
        ad9833_dev_handle->set_wave_freq(ad9833_dev_handle,current_freq,AD9833_REG_FREQ0);
        current_freq += freq_step;
        DELAY_US(77.6);//80us sin : 77.6
    }
    ad9833_dev_handle->reset(ad9833_dev_handle);//ÖØÖÃDDS
}
