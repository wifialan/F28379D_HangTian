
#include "project.h"

Uint16 sdata[2];     // Send data buffer
Uint16 rdata[2];     // Receive data buffer
Uint16 rdata_point;  // Keep track of where we are
// in the data stream to check received data
ESPI* spi_new_dev(enum spi_select_t spi_select)
{
    ESPI* dev;

    dev = (ESPI*)malloc(sizeof(ESPI));
    dev->master = (struct spi_master_t*)malloc(sizeof(struct spi_master_t));

    dev->master->init = &f28379d_spi_init;
    dev->master->write_word = &f28379d_spi_write_16b;
    dev->master->read_word = &f28379d_spi_read_8b;
    dev->master->write_word_8b = &f28379d_spi_write_8b;
    dev->master->select_spi = spi_select;
    dev->master->init(dev);

    return dev;
}

void spi_remove_dev(ESPI* dev)
{
    free(dev);
}

void f28379d_spi_init(ESPI* dev)
{
    //    __spi_init();
    switch (dev->master->select_spi) {
    case SPIA:
        __spia_init();
        break;
    case SPIB:
        __spib_init();
        break;
    case SPIC:
        __spic_init();
        break;
    default:
        break;
    }
}

void f28379d_spi_write_16b(ESPI* dev, uint16_t words)
{
    switch (dev->master->select_spi) {
    case SPIA:
        spia_xmit(words);
        break;
    case SPIB:
        spib_xmit(words);
        break;
    case SPIC:
        spic_xmit(words);
        break;
    default:
        break;
    }
}

void f28379d_spi_write_8b(ESPI* dev, unsigned char words)
{
    switch (dev->master->select_spi) {
    case SPIA:
        spia_xmit_8b(words);
        break;
    case SPIB:
        spib_xmit_8b(words);
        break;
    case SPIC:
        spic_xmit_8b(words);
        break;
    default:
        break;
    }
}

//---------------------------------------------------------------
//SPI TX
//---------------------------------------------------------------
void spia_xmit(uint16_t a)
{
    while( __spia_tx_busy() );
    SpiaRegs.SPITXBUF = a;
}

void spib_xmit(uint16_t a)
{
    while( __spib_tx_busy() );
        SpibRegs.SPITXBUF = a;
}

void spic_xmit(uint16_t a)
{
    while( __spic_tx_busy() );
    SpicRegs.SPITXBUF = a;
}
//发送8位
void spia_xmit_8b(unsigned char a)
{
    while( __spia_tx_busy() );
    SpiaRegs.SPITXBUF = a<<8;
}

void spib_xmit_8b(unsigned char a)
{
    while( __spib_tx_busy() );
    SpibRegs.SPITXBUF = a<<8;
}

void spic_xmit_8b(unsigned char a)
{
    while( __spic_tx_busy() );
    SpicRegs.SPITXBUF = a<<8;
}

//---------------------------------------------------------------
//SPI TX BUSY DETECT
//---------------------------------------------------------------
static int __spia_tx_busy()
{
    if (SpiaRegs.SPISTS.bit.BUFFULL_FLAG == 1)
        return 1;
    else
        return 0;
}

static int __spib_tx_busy()
{
    if (SpibRegs.SPISTS.bit.BUFFULL_FLAG == 1)
        return 1;
    else
        return 0;
}

static int __spic_tx_busy()
{
    if (SpicRegs.SPISTS.bit.BUFFULL_FLAG == 1)
        return 1;
    else
        return 0;
}

unsigned char f28379d_spi_read_8b(ESPI* dev)
{
    unsigned char spi_read_data;
    switch (dev->master->select_spi) {
    case SPIA:
        spi_read_data = spia_rev();
        break;
    case SPIB:
        spi_read_data = spib_rev();
        break;
    case SPIC:
        spi_read_data = spic_rev();
        break;
    default:
        break;
    }
    return spi_read_data;
}
//---------------------------------------------------------------
//SPI Receive
//---------------------------------------------------------------
unsigned char spia_rev()
{
    unsigned char spi_rev_data;
    while( SpiaRegs.SPISTS.bit.INT_FLAG !=1 );
    spi_rev_data = SpiaRegs.SPIRXBUF;
    return spi_rev_data;
}
unsigned char spib_rev()
{
    unsigned char spi_rev_data;
    while( SpibRegs.SPISTS.bit.INT_FLAG !=1 );
    spi_rev_data = SpibRegs.SPIRXBUF;
    return spi_rev_data;
}
unsigned char spic_rev()
{
    unsigned char spi_rev_data;
    while( SpicRegs.SPISTS.bit.INT_FLAG !=1 );
    spi_rev_data = SpicRegs.SPIRXBUF;
    return spi_rev_data;
}

//---------------------------------------------------------------
//SPI INIT
//---------------------------------------------------------------
static void __spia_init(void)
{
    // F28379D的SPIA的引脚共有三组可复用选用,详见 datasheet 中 “GPIO Muxed Pins” Table4-4
    // 第一组: GPIO16-GPIO19 (1)
    // 第二组: GPIO54-GPIO57 (1)
    // 第三组: GPIO58-GPIO61 (15)

    // set SIMO pin
    GPIO_SetupPinMux(54, GPIO_MUX_CPU1, 1);//第三个参数详见 datasheet 中 “GPIO Muxed Pins” Table4-4
    GPIO_SetupPinOptions(54, GPIO_OUTPUT,GPIO_ASYNC);
    // set SOMI pin
    GPIO_SetupPinMux(55, GPIO_MUX_CPU1, 1);
    GPIO_SetupPinOptions(55, GPIO_INPUT, GPIO_ASYNC);
    // set CLK
    GPIO_SetupPinMux(56, GPIO_MUX_CPU1, 1);
    GPIO_SetupPinOptions(56, GPIO_OUTPUT,GPIO_ASYNC);
    // set CS
    GPIO_SetupPinMux(57, GPIO_MUX_CPU1, 1);
    GPIO_SetupPinOptions(57, GPIO_OUTPUT,GPIO_ASYNC);

    SpiaRegs.SPICCR.bit.SPISWRESET = 0;
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;
    SpiaRegs.SPICCR.bit.SPILBK = 0;
    SpiaRegs.SPICCR.bit.SPICHAR = 15;
    SpiaRegs.SPICCR.bit.HS_MODE =   1;

    SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0;
    SpiaRegs.SPICTL.bit.CLK_PHASE = 1;
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpiaRegs.SPICTL.bit.TALK = 1;
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;
    SpiaRegs.SPIBRR.all = 0x02;
#if _FLASH
SpiaRegs.SPIPRI.bit.SOFT = 1;
SpiaRegs.SPIPRI.bit.FREE = 0;//Standard SPI
#endif
SpiaRegs.SPICCR.bit.SPISWRESET = 1;
}


static void __spib_init(void)
{
    // F28379D的SPIB的引脚共有三组可复用选用,详见 datasheet 中 “GPIO Muxed Pins” Table4-4
    // 第一组: GPIO22-GPIO25 (6)       注：GPIO26-27也为独立的CLK和CS信号，具体参考datasheet
    // 第二组: GPIO58-GPIO61 (6)
    // 第三组: GPIO63-GPIO66 (15)

    // set SIMO pin
    GPIO_SetupPinMux(63, GPIO_MUX_CPU1, 15);//第三个参数详见 datasheet 中 “GPIO Muxed Pins” Table4-4
    GPIO_SetupPinOptions(63, GPIO_OUTPUT,GPIO_ASYNC);
    // set SOMI pin
    GPIO_SetupPinMux(64, GPIO_MUX_CPU1, 15);
    GPIO_SetupPinOptions(64, GPIO_INPUT, GPIO_ASYNC);
    // set CLK
    GPIO_SetupPinMux(65, GPIO_MUX_CPU1, 15);
    GPIO_SetupPinOptions(65, GPIO_OUTPUT,GPIO_ASYNC);
    // set CS
    GPIO_SetupPinMux(66, GPIO_MUX_CPU1, 15);
    GPIO_SetupPinOptions(66, GPIO_OUTPUT,GPIO_ASYNC);

    //    SpibRegs.SPIFFTX.all = 0xE040;
    //    SpibRegs.SPIFFRX.all = 0x2044;
    //    SpibRegs.SPIFFCT.all = 0x0;

    SpibRegs.SPICCR.bit.SPISWRESET = 0;
    //Rising edge without delay
    //Data is output on the failing edge of the SPICLK signal.
    //Input data is latched on the rising edge of the SPICLK signal.
    SpibRegs.SPICCR.bit.CLKPOLARITY = 0;
    SpibRegs.SPICTL.bit.CLK_PHASE = 1;

    SpibRegs.SPICCR.bit.SPILBK = 0;
    SpibRegs.SPICCR.bit.SPICHAR = 15;
    SpibRegs.SPICCR.bit.HS_MODE =   1;

    SpibRegs.SPICTL.bit.OVERRUNINTENA = 0;
    SpibRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpibRegs.SPICTL.bit.TALK = 1;
    SpibRegs.SPICTL.bit.SPIINTENA = 0;
    SpibRegs.SPIBRR.all = 0x04;
#if _FLASH
SpibRegs.SPIPRI.bit.SOFT = 1;
SpibRegs.SPIPRI.bit.FREE = 0;//Standard SPI
#endif
SpibRegs.SPICCR.bit.SPISWRESET = 1;
}


static void __spic_init(void)
{
    // F28379D的SPIC的引脚共有三组可复用选用,详见 datasheet 中 “GPIO Muxed Pins” Table4-4
    // 第一组: GPIO50-GPIO53 (6)       注：GPIO26-27也为独立的CLK和CS信号，具体参考datasheet
    // 第二组: GPIO69-GPIO72 (15)
    // 第三组: GPIO100-GPIO103 (15)

    // set SIMO pin
    GPIO_SetupPinMux(50, GPIO_MUX_CPU1, 6);//第三个参数详见 datasheet 中 “GPIO Muxed Pins” Table4-4
    GPIO_SetupPinOptions(50, GPIO_OUTPUT,GPIO_ASYNC);
    // set SOMI pin
    GPIO_SetupPinMux(51, GPIO_MUX_CPU1, 6);
    GPIO_SetupPinOptions(51, GPIO_INPUT, GPIO_ASYNC);
    // set CLK
    GPIO_SetupPinMux(52, GPIO_MUX_CPU1, 6);
    GPIO_SetupPinOptions(52, GPIO_OUTPUT,GPIO_ASYNC);
    // set CS
    GPIO_SetupPinMux(53, GPIO_MUX_CPU1, 6);
    GPIO_SetupPinOptions(53, GPIO_OUTPUT,GPIO_ASYNC);

    SpicRegs.SPICCR.bit.SPISWRESET = 0;
    //Rising edge without delay
    //Data is output on the failing edge of the SPICLK signal.
    //Input data is latched on the rising edge of the SPICLK signal.
    SpicRegs.SPICCR.bit.CLKPOLARITY = 0;
    SpicRegs.SPICTL.bit.CLK_PHASE = 1;

    SpicRegs.SPICCR.bit.SPILBK = 0;
    SpicRegs.SPICCR.bit.SPICHAR = 15;
    SpicRegs.SPICCR.bit.HS_MODE =   1;

    SpicRegs.SPICTL.bit.OVERRUNINTENA = 0;
    SpicRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpicRegs.SPICTL.bit.TALK = 1;
    SpicRegs.SPICTL.bit.SPIINTENA = 0;
    SpicRegs.SPIBRR.all = 0x04;
#if _FLASH
SpicRegs.SPIPRI.bit.SOFT = 1;
SpicRegs.SPIPRI.bit.FREE = 0;//Standard SPI
#endif
SpicRegs.SPICCR.bit.SPISWRESET = 1;
}


interrupt void spiTxFifoIsr(void)
{
    Uint16 i;
    for(i=0;i<2;i++)
    {
        SpiaRegs.SPITXBUF=sdata[i];      // Send data
    }

    for(i=0;i<2;i++)                    // Increment data for next cycle
    {
        sdata[i] = sdata[i] + 1;
    }

    SpiaRegs.SPIFFTX.bit.TXFFINTCLR=1;  // Clear Interrupt flag
    PieCtrlRegs.PIEACK.all|=0x20;       // Issue PIE ACK
}

interrupt void spiRxFifoIsr(void)
{
    Uint16 i;
    for(i=0;i<2;i++)
    {
        rdata[i]=SpiaRegs.SPIRXBUF;     // Read data
    }
    for(i=0;i<2;i++)                    // Check received data
    {

    }
    rdata_point++;
    SpiaRegs.SPIFFRX.bit.RXFFOVFCLR=1;  // Clear Overflow flag
    SpiaRegs.SPIFFRX.bit.RXFFINTCLR=1;  // Clear Interrupt flag
    PieCtrlRegs.PIEACK.all|=0x20;       // Issue PIE ack
}




