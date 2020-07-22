
#ifndef INC_SCI_H_
#define INC_SCI_H_

#include "project.h"

#define                         CPU_FREQ    200E6
#define                         LSPCLK_FREQ CPU_FREQ/8
#define                         SCI_FREQ    115200
#define                         SCI_PRD     (LSPCLK_FREQ/(SCI_FREQ*8))-1
#define                         sci_RECV_BUFFER_MAX            256

typedef  struct sci_t ESCI;

enum sci_id_t
{
    SCIA,
    SCIB,
    SCIC,
    SCID,
};

struct sci_hw_t
{
    uint16_t rx_pin;
    uint16_t tx_pin;
};

struct sci_config_t
{

    uint32_t  baud_rate;
    uint16_t  word_length;
    uint16_t  stop_bits;
    uint16_t  parity;
    uint16_t  hardware_flow_ctrl;
    uint16_t mode;
};
struct sci_msg_t{

    uint32_t buffer_deep;
    char *buffer_rom;

};

struct sci_master_t
{
    void (*init)(ESCI*, uint32_t);
    void (*write_string)(ESCI*, char* , int);
    void (*write_byte)(ESCI*, uint8_t);
    void (*clear_buffer)(ESCI*);
    void (*push_to_ring)(ESCI*, char*, uint32_t);
    void (*pop_from_ring)(ESCI*, char*, uint32_t);
};



struct  sci_t
{
    enum sci_id_t id;
    struct sci_t *self;
    struct sci_hw_t hw;
    struct sci_config_t sci_cfg;
    struct sci_master_t *master;
    struct sci_msg_t *msg;

};

extern ESCI* sci_new_dev(enum sci_id_t sci_id, uint32_t baud);
extern void sci_remove_dev(ESCI* sci);

static void __scid_fifo_init(uint32_t baud);
static void __scic_fifo_init(uint32_t baud);
static void __scib_fifo_init(uint32_t baud);
static void __scia_fifo_init(uint32_t baud);
static void __scid_msg(char * msg, int length);
static void __scid_xmit(int a);
static void __scic_msg(char * msg, int length);
static void __scic_xmit(int a);
static void __scib_msg(char * msg, int length);
static void __scib_xmit(int a);
static void __scia_msg(char * msg, int length);
static void __scia_xmit(int a);
void f28379d_sci_push_to_ring_buffer(ESCI* sci, char* buffer, uint32_t length);
void f28379d_sci_pop_from_ring_buffer(ESCI* sci, char* buffer, uint32_t length);
void f28379d_sci_clear_buffer(ESCI* sci);
void f28379d_sci_write_string(ESCI* sci, char* str, int length);
void f28379d_sci_write_byte(ESCI *sci, uint8_t byte);
void f28379d_sci_init(ESCI* sci, uint32_t baud);

interrupt void scidRxFifoIsr(void);
interrupt void scicRxFifoIsr(void);
interrupt void scibRxFifoIsr(void);
interrupt void sciaRxFifoIsr(void);


#endif /* INC_SCI_H_ */
