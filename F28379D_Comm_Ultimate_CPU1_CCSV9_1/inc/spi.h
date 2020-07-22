/*
 * spi.h
 *
 *  Created on: 2018骞�9鏈�11鏃�
 *      Author: root
 */

#ifndef SRC_SPI_H_
#define SRC_SPI_H_

#include "F2837xD_spi.h"

typedef struct spi_t ESPI;

enum spi_select_t{
    SPIA,SPIB,SPIC
};

struct spi_config_t {
};

struct spi_master_t {

    void (*init)(ESPI*);
    void (*write_word)(ESPI*, uint16_t);
    void (*write_word_8b)(ESPI*, unsigned char);
    unsigned char (*read_word)(ESPI*);
    //enum spi_select_t spi_select;
    uint16_t select_spi;

};

struct spi_t {
    ESPI* self;
    struct spi_master_t *master;
    struct spi_config_t *config;
};

extern ESPI* spi_new_dev(enum spi_select_t spi_select);
extern void spi_remove_dev(ESPI* dev);
void f28379d_spi_init(ESPI* dev);
void f28379d_spi_write_16b(ESPI* dev, uint16_t words);
void f28379d_spi_write_8b(ESPI* dev, unsigned char words);
unsigned char f28379d_spi_read_8b(ESPI* dev);
void spia_xmit(uint16_t a);
void spib_xmit(uint16_t a);
void spic_xmit(uint16_t a);
static int __spia_tx_busy();
static int __spib_tx_busy();
static int __spic_tx_busy();

unsigned char spia_rev();
unsigned char spib_rev();
unsigned char spic_rev();
void spia_xmit_8b(unsigned char a);
void spib_xmit_8b(unsigned char a);
void spic_xmit_8b(unsigned char a);

static void __spia_init(void);
static void __spib_init(void);
static void __spic_init(void);

unsigned char spib_rev();


#endif /* SRC_SPI_H_ */
