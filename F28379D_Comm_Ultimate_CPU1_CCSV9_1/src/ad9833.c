/*
 * ad9833.c
 *
 *  Created on: 2018��11��30��
 *      Author: multimicro
 */
#include "project.h"

#define         MHz             1000000UL

#define         AD9833_OSC_20MHz      (20UL*MHz)
#define         AD9833_OSC_25MHz      (25UL*MHz)

void ad9833_init( AD9833 *dev, enum spi_select_t spi_select )
{
    dev->set_wave_freq      =   &ad9833_set_wave_freq;
    dev->set_wave_phase     =   &ad9833_set_wave_phase;
    dev->set_wave_type      =   &ad9833_set_wave_type;
    dev->set_wave_para      =   &ad9833_set_para;
    dev->write_reg          =   &ad9833_write_reg;
    dev->init_device        =   &ad9833_device_init;
    dev->reset              =   &ad9833_reset;

    dev->hw->spi_select = spi_select;

    dev->init_device(dev,spi_select);
}

void ad9833_device_init(AD9833 *dev,enum spi_select_t spi_select)
{
    dev->hw->spi = spi_new_dev(spi_select);
    dev->write_reg(dev, AD9833_RESET);
}

void ad9833_write_reg(AD9833 *dev, Uint16 data)
{
    dev->hw->spi->master->select_spi = dev->hw->spi_select;//ѡ��SPI����
    dev->hw->spi->master->write_word(dev->hw->spi, data);
}

void ad9833_set_para(AD9833 *dev, Uint32 freq, Uint16 freq_reg, Uint16 phase, enum ad9833_wavetype_t wave_type)
{
    dev->set_wave_freq(dev, freq,freq_reg);
    dev->set_wave_phase(dev, phase);
    dev->set_wave_type(dev, wave_type);
}

void ad9833_set_wave_freq(AD9833 *dev, Uint32 freq, Uint16 freq_reg)
{
    Uint32 dds_frequence_data;
    Uint16 dds_frequence_low;
    Uint16 dds_frequence_high;

    //��   268435456 / ( 25 * MHz ) = 10.73741824 �ó�   ע��2��28�η�Ϊ268435456
    //20MHz�ľ���Ӧ��Ϊ��268435456 / ( 20 * MHz ) = 13.4217728
    dds_frequence_data      =   freq * 13.4217728;
    dds_frequence_low       =   dds_frequence_data & 0x3FFF;
    dds_frequence_data      =   dds_frequence_data >> 14;
    dds_frequence_high      =   dds_frequence_data & 0x3FFF;

    //    dds_frequence_low       |=  AD9833_REG_FREQ0;
    //    dds_frequence_high      |=  AD9833_REG_FREQ0;
    dds_frequence_low       |=  freq_reg;
    dds_frequence_high      |=  freq_reg;

    dev->write_reg(dev,AD9833_B28|0x0028);//|0x0020
    dev->write_reg(dev,dds_frequence_low);
    dev->write_reg(dev,dds_frequence_high);
}

void ad9833_set_wave_phase(AD9833 *dev, Uint16 phase)
{
    phase = phase | AD9833_REG_PHASE0;
    dev->write_reg( dev, phase );
}

void ad9833_set_wave_type(AD9833 *dev, enum ad9833_wavetype_t wave_type)
{
    switch (wave_type) {
    case 0: dev->write_reg(dev, WAVE_SIN);break;
    case 1: dev->write_reg(dev, WAVE_SQU);break;
    case 2: dev->write_reg(dev, WAVE_TRI);break;
    default: break;
    }
}

void ad9833_reset(AD9833 *dev)
{
    dev->write_reg(dev,AD9833_RESET);
}
