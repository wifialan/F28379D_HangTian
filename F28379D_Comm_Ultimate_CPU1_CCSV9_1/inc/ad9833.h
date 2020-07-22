/*
 * ad9833.h
 *
 *  Created on: 2018Äê11ÔÂ30ÈÕ
 *      Author: multimicro
 */

#ifndef INC_AD9833_H_
#define INC_AD9833_H_


/* Registers */

#define AD9833_REG_CMD      (0 << 14)
#define AD9833_REG_FREQ0    (1 << 14)
#define AD9833_REG_FREQ1    (2 << 14)
#define AD9833_REG_PHASE0   (6 << 13)
#define AD9833_REG_PHASE1   (7 << 13)

/* Command Control Bits */

#define AD9833_B28          (1 << 13)
#define AD9833_HLB          (1 << 12)
#define AD9833_FSEL0        (0 << 11)
#define AD9833_FSEL1        (1 << 11)
#define AD9833_PSEL0        (0 << 10)
#define AD9833_PSEL1        (1 << 10)
#define AD9833_PIN_SW       (1 << 9)
#define AD9833_RESET        (1 << 8)
#define AD9833_SLEEP1       (1 << 7)
#define AD9833_SLEEP12      (1 << 6)
#define AD9833_OPBITEN      (1 << 5)
#define AD9833_SIGN_PIB     (1 << 4)
#define AD9833_DIV2         (1 << 3)
#define AD9833_MODE         (1 << 1)

#define AD9833_OUT_SINUS    ((0 << 5) | (0 << 1) | (0 << 3))
#define AD9833_OUT_TRIANGLE ((0 << 5) | (1 << 1) | (0 << 3))
#define AD9833_OUT_MSB      ((1 << 5) | (0 << 1) | (1 << 3))
#define AD9833_OUT_MSB2     ((1 << 5) | (0 << 1) | (0 << 3))

#define             Hz                          1ul
#define             KHz                         1000ul
#define             WAVE_SIN                    0x2000
#define             WAVE_SQU                    0x2020
#define             WAVE_TRI                    0x2002

typedef struct ad9833_t AD9833;

enum ad9833_wavetype_t{
    SIN,SQU,TRI
};

struct ad9833_hw_t {

    uint16_t clk;
    Uint16 sdi;
    Uint16 fsy;
    ESPI *spi;
    uint16_t  spi_select;
};

struct ad9833_t {

    struct ad9833_hw_t *hw;
    struct ad9833_t *self;
    enum ad9833_wavetype_t wave_type;

    Uint16 delay;

    void (*reset)(struct ad9833_t *self);
    void (*write_reg)( struct ad9833_t *self, Uint16 reg_value );
    void (*init_device)( struct ad9833_t *self ,enum spi_select_t spi_select);
    void (*set_wave_freq)( struct ad9833_t *self, Uint32 freq, Uint16 freq_reg );
    void (*set_wave_type)( struct ad9833_t *self, enum ad9833_wavetype_t wave_type );
    void (*set_wave_phase)( struct ad9833_t *self, Uint16 phase );
    void (*set_wave_para)( struct ad9833_t *self, Uint32 freqs_data, Uint16 freq_reg, Uint16 phase, enum ad9833_wavetype_t wave_type );
};

void ad9833_set_para( struct ad9833_t *self,Uint32 freq, Uint16 freq_reg, Uint16 phase, enum ad9833_wavetype_t wave_type );
void ad9833_device_init( struct ad9833_t *self, enum spi_select_t spi_select);
void ad9833_set_wave_freq( struct ad9833_t *self , Uint32 freq ,Uint16 freq_reg);
void ad9833_set_wave_type( struct ad9833_t *self, enum ad9833_wavetype_t wave_type );
void ad9833_set_wave_phase( struct ad9833_t *self, Uint16 phase );
void ad9833_write_reg( struct ad9833_t *self, Uint16 data );
void ad9833_delay( struct ad9833_t *self );
void ad9833_gpio_init( void );
void ad9833_init( AD9833 *dev ,enum spi_select_t spi_select);
void ad9833_reset(AD9833 *dev);

#endif /* INC_AD9833_H_ */
