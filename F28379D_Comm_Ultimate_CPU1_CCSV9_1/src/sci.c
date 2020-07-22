
#include "project.h"


#ifndef CPU2_CTRL_UARTB
unsigned char rdataA[30];
uint32_t sci_counter=0;
#endif

extern void hex2int(unsigned char* ptrchar,uint32_t *ptrint);
extern uint32_t data_in_uint;
extern uint32_t cpu1_rev_cpu2_data;

extern struct sys_status sys;

void init_sci()
{
#ifdef CPU2_CTRL_UARTB
    GPIO_SetupPinMux(87, GPIO_MUX_CPU2, 5);
    GPIO_SetupPinOptions(87, GPIO_INPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(86, GPIO_MUX_CPU2, 5);
    GPIO_SetupPinOptions(86, GPIO_OUTPUT, GPIO_ASYNC);

    EALLOW;
    DevCfgRegs.CPUSEL5.bit.SCI_B = 1;// Transfer ownership of SCI_B to CPU02
    EDIS;

#endif
}

#ifndef CPU2_CTRL_UARTB
ESCI* sci_new_dev(enum sci_id_t sci_id, uint32_t baud)
{
    ESCI* dev;

    dev = (ESCI*)malloc(sizeof(ESCI));

    dev->id = sci_id;
    dev->master = (struct sci_master_t*)malloc(sizeof(struct sci_master_t));
    dev->msg->buffer_deep = 1024;
    dev->msg->buffer_rom = (char*)malloc(sizeof(char)*dev->msg->buffer_deep);
    //	dev->msg->ring_io = ring_new_dev(dev->msg->buffer_deep, (void*)dev->msg->buffer_rom);
    dev->master->init = &f28379d_sci_init;
    dev->master->write_byte = &f28379d_sci_write_byte;
    dev->master->write_string = &f28379d_sci_write_string;
    dev->master->clear_buffer = &f28379d_sci_clear_buffer;
    //	dev->master->push_to_ring = &f28379d_sci_push_to_ring_buffer;
    //	dev->master->pop_from_ring =&f28379d_sci_pop_from_ring_buffer;

    dev->master->init(dev, baud);
    return dev;
}

void sci_remove_dev(ESCI* sci)
{
    free(sci);
}

void f28379d_sci_init(ESCI* sci, uint32_t baud)
{
    switch(sci->id) {
    case SCIA:
        __scia_fifo_init(baud);
        break;

    case SCIB:
        __scib_fifo_init(baud);
        break;

    case SCIC:
        __scic_fifo_init(baud);
        break;

    case SCID:
        __scid_fifo_init(baud);
        break;
    }
}

void f28379d_sci_write_byte(ESCI *sci, unsigned char byte)
{
    switch(sci->id) {
    case SCIA:
        __scia_xmit(byte);
        break;

    case SCIB:
        __scib_xmit(byte);
        break;

    case SCIC:
        __scic_xmit(byte);
        break;

    case SCID:
        __scid_xmit(byte);
        break;
    }
}

void f28379d_sci_write_string(ESCI* sci, unsigned char* str, int length)
{
    switch(sci->id) {
    case SCIA:
        __scia_msg(str, length);
        break;

    case SCIB:
        __scib_msg(str, length);
        break;

    case SCIC:
        __scic_msg(str, length);
        break;

    case SCID:
        __scid_msg(str, length);
        break;
    }
}

void f28379d_sci_clear_buffer(ESCI* sci)
{

}

//void f28379d_sci_push_to_ring_buffer(ESCI* sci, char* buffer, uint32_t length)
//{
//	sci->msg->ring_io->master->push(sci->msg->ring_io, (void*)buffer, length);
//}
//
//void f28379d_sci_pop_from_ring_buffer(ESCI* sci, char* buffer, uint32_t length)
//{
//	sci->msg->ring_io->master->pop(sci->msg->ring_io, (void*)buffer, length);
//	buffer = (char*)buffer;
//}


static void __scia_xmit(int a)
{
    while( SciaRegs.SCICTL2.bit.TXRDY != 1 );
    SciaRegs.SCITXBUF.all=a & 0x00FF;

}

static void __scia_msg(char * msg, int length)
{
    int i;
    i = 0;

    if (length < 0) {
        while(msg[i] != '\0') {
            __scia_xmit(msg[i]);
            i++;
        }
    }else{
        for (i = 0; i < length; i++)
            __scia_xmit(msg[i]);
    }
}

static void __scib_xmit(int a)
{
    while( ScibRegs.SCICTL2.bit.TXRDY != 1 );
    ScibRegs.SCITXBUF.all=a & 0x00FF;

}

static void __scib_msg(char * msg, int length)
{
    int i;
    i = 0;

    if (length < 0) {
        while(msg[i] != '\0') {
            __scib_xmit(msg[i]);
            i++;
        }
    }else{
        for (i = 0; i < length; i++)
            __scib_xmit(msg[i]);
    }

}

static void __scic_xmit(int a)
{
    while( ScicRegs.SCICTL2.bit.TXRDY != 1 );
    ScicRegs.SCITXBUF.all=a & 0x00FF;

}

static void __scic_msg(char * msg, int length)
{
    int i;
    i = 0;

    if (length < 0) {
        while(msg[i] != '\0') {
            __scic_xmit(msg[i]);
            i++;
        }
    }else{
        for (i = 0; i < length; i++)
            __scic_xmit(msg[i]);
    }

}

static void __scid_xmit(int a)
{
    while( ScidRegs.SCICTL2.bit.TXRDY != 1 );
    ScidRegs.SCITXBUF.all=a & 0x00FF;

}

static void __scid_msg(char * msg, int length)
{

    int i;
    i = 0;
    if (length < 0) {
        while(msg[i] != '\0') {
            __scid_xmit(msg[i]);
            i++;
        }
    }else{
        for (i = 0; i < length; i++)
            __scid_xmit(msg[i]);
    }

}


interrupt void sciaRxFifoIsr(void)
{
    Uint16 i;

    for(i=0;i<2;i++)
    {
        //rdataA[i]=SciaRegs.SCIRXBUF.all;  // Read data
    }


    PieCtrlRegs.PIEACK.all|=0x100;       // Issue PIE ack
}

interrupt void scibRxFifoIsr(void)
{

    rdataA[sci_counter]=ScibRegs.SCIRXBUF.all;  // Read data
    sci_counter = ++ sci_counter % SCI_RECE_MAX;
    if (sci_counter == 8) {
        sci_counter = 0;
        hex2int(rdataA, &data_in_uint);
        cpu1_rev_cpu2_data = data_in_uint;
//        ipc_send_data(data_in_uint);//通过IPC传输数据给CPU2
        memset(rdataA, '\0', sizeof(rdataA));
        memset(rdataA, '\0', sizeof(rdataA));
        data_in_uint = 0;
        sys.send_or_rece_state = STATUS_SEND;
    }

    ScibRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear Overflow flag
    ScibRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag
    PieCtrlRegs.PIEACK.all|=0x100;       // Issue PIE ack
}

interrupt void scicRxFifoIsr(void)
{
    Uint16 i;
    for(i=0;i<2;i++)
    {
        //rdataA[i]=ScicRegs.SCIRXBUF.all;  // Read data
    }


    PieCtrlRegs.PIEACK.all|=0x100;       // Issue PIE ack
}

interrupt void scidRxFifoIsr(void)
{
    Uint16 i;
    for(i=0;i<2;i++)
    {
        //rdataA[i]=ScidRegs.SCIRXBUF.all;  // Read data
    }


    PieCtrlRegs.PIEACK.all|=0x100;       // Issue PIE ack
}

static void __scia_fifo_init(uint32_t baud)
{

    unsigned long long cpu_freq = 200E6;
    unsigned long long lspclk_freq = cpu_freq/4;
    unsigned long sci_bdr = (lspclk_freq/(baud*8))-1;
    unsigned short sci_bdr_h = 0;
    unsigned short sci_bdr_l = 0;

    if ( sci_bdr >= 0xFF ) {
        sci_bdr_l = sci_bdr & 0x00FF;
        sci_bdr_h = (sci_bdr & 0xFF00) >> 8;
    }else if( sci_bdr < 0xFF) {
        sci_bdr_l = sci_bdr & 0x00FF;
    }

    GPIO_SetupPinMux(85, GPIO_MUX_CPU1, 5);
    GPIO_SetupPinOptions(85, GPIO_INPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(84, GPIO_MUX_CPU1, 5);
    GPIO_SetupPinOptions(84, GPIO_OUTPUT, GPIO_ASYNC);

    SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
    // No parity,8 char bits,
    // async mode, idle-line protocol
    SciaRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
    // Disable RX ERR, SLEEP, TXWAKE
    SciaRegs.SCICTL2.bit.TXINTENA =1;
    SciaRegs.SCICTL2.bit.RXBKINTENA =1;
    SciaRegs.SCIHBAUD.all = sci_bdr_h;
    SciaRegs.SCILBAUD.all = sci_bdr_l;
    ScibRegs.SCICCR.bit.LOOPBKENA = 0; // Disable loop back
    ScibRegs.SCICTL2.bit.RXBKINTENA =   1;
    ScibRegs.SCICTL2.bit.TXINTENA   =   0;

    ScibRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset

}

static void __scib_fifo_init(uint32_t baud)
{
    unsigned long long cpu_freq = 200E6;
    unsigned long long lspclk_freq = cpu_freq/4;
    unsigned long sci_bdr = (lspclk_freq/(baud*8))-1;
    unsigned short sci_bdr_h = 0;
    unsigned short sci_bdr_l = 0;

    if ( sci_bdr >= 0xFF ) {
        sci_bdr_l = sci_bdr & 0x00FF;
        sci_bdr_h = (sci_bdr & 0xFF00) >> 8;
    }else if( sci_bdr < 0xFF) {
        sci_bdr_l = sci_bdr & 0x00FF;
    }

    GPIO_SetupPinMux(87, GPIO_MUX_CPU1, 5);
    GPIO_SetupPinOptions(87, GPIO_INPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(86, GPIO_MUX_CPU1, 5);
    GPIO_SetupPinOptions(86, GPIO_OUTPUT, GPIO_ASYNC);

    ScibRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
        // No parity,8 char bits,
        // async mode, idle-line protocol
        ScibRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
        // Disable RX ERR, SLEEP, TXWAKE
        ScibRegs.SCICTL2.bit.TXINTENA =1;
        ScibRegs.SCICTL2.bit.RXBKINTENA =1;
        ScibRegs.SCIHBAUD.all = sci_bdr_h;
        ScibRegs.SCILBAUD.all = sci_bdr_l;
        ScibRegs.SCICCR.bit.LOOPBKENA = 0; // disable loop back
    //    ScibRegs.SCICTL2.bit.RXBKINTENA =   1;
    //    ScibRegs.SCICTL2.bit.TXINTENA   =   0;//Without FIFO method

        ScibRegs.SCIFFTX.all = 0xE040;
        ScibRegs.SCIFFRX.all = 0x0021;
        ScibRegs.SCIFFCT.all = 0x00;

        ScibRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset
        //    ScibRegs.SCIFFTX.bit.TXFIFORESET = 1;
        ScibRegs.SCIFFRX.bit.RXFIFORESET = 1;

}

static void __scic_fifo_init(uint32_t baud)
{
    unsigned long long cpu_freq = 200E6;
    unsigned long long lspclk_freq = cpu_freq/4;
    unsigned long sci_bdr = (lspclk_freq/(baud*8))-1;
    unsigned short sci_bdr_h = 0;
    unsigned short sci_bdr_l = 0;

    if ( sci_bdr >= 0xFF ) {
        sci_bdr_l = sci_bdr & 0x00FF;
        sci_bdr_h = (sci_bdr & 0xFF00) >> 8;
    }else if( sci_bdr < 0xFF) {
        sci_bdr_l = sci_bdr & 0x00FF;
    }

    GPIO_SetupPinMux(90, GPIO_MUX_CPU1, 6);
    GPIO_SetupPinOptions(90, GPIO_INPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(89, GPIO_MUX_CPU1, 6);
    GPIO_SetupPinOptions(89, GPIO_OUTPUT, GPIO_ASYNC);
    ScicRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
    // No parity,8 char bits,
    // async mode, idle-line protocol
    ScicRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
    // Disable RX ERR, SLEEP, TXWAKE
    ScicRegs.SCICTL2.bit.TXINTENA =1;
    ScicRegs.SCICTL2.bit.RXBKINTENA =1;
    ScicRegs.SCIHBAUD.all = sci_bdr_h;
    ScicRegs.SCILBAUD.all = sci_bdr_l;
    ScibRegs.SCICCR.bit.LOOPBKENA = 0; // Disable loop back
    ScibRegs.SCICTL2.bit.RXBKINTENA =   1;
    ScibRegs.SCICTL2.bit.TXINTENA   =   0;

    ScibRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset

}

static void __scid_fifo_init(uint32_t baud)
{
    unsigned long long cpu_freq = 200E6;
    unsigned long long lspclk_freq = cpu_freq/4;
    unsigned long sci_bdr = (lspclk_freq/(baud*8))-1;
    unsigned short sci_bdr_h = 0;
    unsigned short sci_bdr_l = 0;

    if ( sci_bdr >= 0xFF ) {
        sci_bdr_l = sci_bdr & 0x00FF;
        sci_bdr_h = (sci_bdr & 0xFF00) >> 8;
    }else if( sci_bdr < 0xFF) {
        sci_bdr_l = sci_bdr & 0x00FF;
    }

    GPIO_SetupPinMux(94, GPIO_MUX_CPU1, 6);
    GPIO_SetupPinOptions(94, GPIO_INPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(93, GPIO_MUX_CPU1, 6);
    GPIO_SetupPinOptions(93, GPIO_OUTPUT, GPIO_ASYNC);
    ScidRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
    // No parity,8 char bits,
    // async mode, idle-line protocol
    ScidRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
    // Disable RX ERR, SLEEP, TXWAKE
    ScidRegs.SCICTL2.bit.TXINTENA =1;
    ScidRegs.SCICTL2.bit.RXBKINTENA =1;
    ScidRegs.SCIHBAUD.all = sci_bdr_h;
    ScidRegs.SCILBAUD.all = sci_bdr_l;
    ScibRegs.SCICCR.bit.LOOPBKENA = 0; // Disable loop back
    ScibRegs.SCICTL2.bit.RXBKINTENA =   1;
    ScibRegs.SCICTL2.bit.TXINTENA   =   0;

    ScibRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset

}

#endif

