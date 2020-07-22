/**
 * \brief   This project about <program name> TMS320F28379D.
 *
 * \License  THIS FILE IS PART OF MULTIBEANS PROJECT ;
 *           all of the files  - The core part of the project;
 *           THIS PROGRAM IS FREE SOFTWARE, JUST NEED GPL 3.0 LICENSE;
 *           YOU SHOULD HAVE RECEIVED A COPY OF WTFPL LICENSE, IF NOT,
 *           MULTIBEANS WILL TAKE APPROPRIATE MEASURES.
 *
 *                ________________     ___           _________________
 *               |    __    __    |   |   |         |______     ______|
 *               |   |  |  |  |   |   |   |                |   |
 *               |   |  |__|  |   |   |   |________        |   |
 *               |___|        |___|   |____________|       |___|
 *
 *                               MULTIBEANS ORG.
 *                     Homepage: http://www.mltbns.com/
 *
 *           * You can download the license on our Github. ->
 *           * -> https://github.com/multibeans  <-
 *           * Copyright (c) 2013-2018 MULTIBEANS ORG. http://www.mltbns.com/
 *           * Copyright (c) 2018 Tian Zhiying(Alan Tian: # tianzhiying@mltbns.com).
 *
 *  \note    void.
 ****************************************************************************/
/*                                                                          */
/*  @File       : main.c                                                    */
/*  @Revision   : Ver 1.0.                                                  */
/*  @Date       : 2018.11.30 Realse.                                        */
/*  @Belong     : PROJECT.                                                  */
/*  @GitHub     :                                                           */
/*  @ASCII : (GBK/GB2312) in Windows x64. CCSV8                             */
/****************************************************************************/
/*  @Attention:                                                             */
/*  ---------------------------------------------------------------------   */
/*  |    Data    |  Behavior |     Offer     |          Content         |   */
/*  |------------|-----------|---------------|--------------------------|   */
/*  | 2018.11.30 |   create  | Alan tian(M)  |creat the project template|   */
/*  ---------------------------------------------------------------------   */
/*                                                            MULTIBEANS.   */
/****************************************************************************/

//! This example demonstrates how to make use of the EPWM peripherals from
//! CPU2.  Device clocking (PLL) and GPIO setup are done using CPU1,
//! while all other configuration of the peripherals is done using CPU2.
//! Dont't forget transfer ownership of EPWM peripherals to CPU02

#include "project.h"
void hex2int(unsigned char* ptrchar,uint32_t *ptrint);
void ipc_send_data(uint32_t data);
extern void floatTochar(float _data, char *_str, int decimal_point);

inline void start_plause_channel_1(int num);
inline void start_plause_channel_2(int num);
inline void start_plause_channel_3(int num);

__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);

void detect_program_begin();

ESCI    *scib_pdev;
extern ECAP_DEV ecap;

uint32_t cpu2_rev_cpu1_data;
char comm_data[8];

//在Debug模式下，该宏定义设置为4.74，表示4.74us，这样可以控制IO打出100KHz的脉冲
//在FLASH模式下，该宏定义设置为4.60，表示4.60us，这样可以控制IO打出100KHz的脉冲
//添加OFFSET的原因在于for循环会消耗一定的时间，这个偏移就是来平衡该时间消耗，从而使得高电平持续时间和低电平持续时间相同
#ifdef _FLASH
#define         FREQ_PLAUSE_US              4.57    // DEBUG 4.72  |  FLASH 4.58
#define         FREQ_PLAUSE_US_OFFSET       0.08
#else
#define         FREQ_PLAUSE_US              4.72    // DEBUG 4.72  |  FLASH 4.58
#define         FREQ_PLAUSE_US_OFFSET       0.04
#endif

#define         TIME_OFFSET_US              150
#define         CPU_TIMER_MS                1000UL
#define         PLAUSE_NUMBER               5

void main(void)
{

    f28379d_system_init();

#ifdef _FLASH
    // Let CPU1 know that CPU2 is ready
    IpcRegs.IPCSET.bit.IPC7 = 1;       // Set IPC7 to release CPU1
#endif
    init_ecap();
    GPIO_WritePin(PLAUSE_CHANNEL_1, 0);
    GPIO_WritePin(PLAUSE_CHANNEL_2, 0);
    GPIO_WritePin(PLAUSE_CHANNEL_3, 0);
    scib_pdev = sci_new_dev(SCIB,115200);
    //    __scib_fifo_init(115200);

    // 响应并清除IPC1中断接收标志位
    IpcRegs.IPCACK.bit.IPC1 = 1;

    scib_pdev->master->write_string(scib_pdev,"Start\r\n",-1);

    //        StartCpuTimer0();
    StartCpuTimer1();

    // Step 6. IDLE loop. Just sit and loop forever (optional):
    while(1)
    {
        //    start_plause_channel_1(1);
        detect_program_begin();
        DELAY_MS(100);

    }
}

void f28379d_system_init(void)
{

    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the F2837xD_SysCtrl.c file.
    InitSysCtrl();

    // Step 2. Initialize GPIO:
    // This example function is found in the F2837xD_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    //    InitGpio(); // Skipped for this example

    EALLOW;
    //TODO Add code to configure GPADIR through IPC
    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    DINT;

    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the F2837xD_PieCtrl.c file.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in F2837xD_DefaultIsr.c.
    // This function is found in F2837xD_PieVect.c.
    InitPieVectTable();

    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.IPC1_INT = &ipc1_isr;      // IPC1 interrupt
    //    PieVectTable.SCIA_RX_INT = &sciaRxFifoIsr;
    PieVectTable.SCIB_RX_INT = &scibRxFifoIsr;
    //    PieVectTable.SCIC_RX_INT = &scicRxFifoIsr;
    //    PieVectTable.SCID_RX_INT = &scidRxFifoIsr;
    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    PieVectTable.TIMER1_INT = &cpu_timer1_isr;
    PieVectTable.ECAP1_INT = &ecap1_isr_signal;
    PieVectTable.ECAP2_INT = &ecap2_isr_signal;
    PieVectTable.ECAP3_INT = &ecap3_isr_signal;

    EDIS;   // This is needed to disable write to EALLOW protected registers

    // PIE Channel Mapping
    IpcRegs.IPCCLR.all = 0xFFFFFFFF;

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;      // Enable the PIE block
    PieCtrlRegs.PIEIER1.bit.INTx14 = 1;     // Enable IPC1 ISR
    //    PieCtrlRegs.PIEIFR1.bit.INTx13 = 1;     // Force first IPC0 transfer
    PieCtrlRegs.PIEIER9.bit.INTx3=1;        // PIE Group 9, INT3, SCIB_RX
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;      // PIE Group 1, INT7, Timer 0
    // PieCtrlRegs.PIEIER9.bit.INTx4=0;     // PIE Group 9, INT4, SCIB_TX

    PieCtrlRegs.PIEIER4.bit.INTx1 = 1;      // PIE Group 4, INT1, ECAP1
    PieCtrlRegs.PIEIER4.bit.INTx2 = 1;
    PieCtrlRegs.PIEIER4.bit.INTx3 = 1;


    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1 | PIEACK_GROUP4 | PIEACK_GROUP9;

    IER |= M_INT1 | M_INT4 | M_INT9; // Enable CPU INT
    IER |= M_INT13;                  // CPU TIMER 1
    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0, 200, 100*CPU_TIMER_MS);//100ms
    ConfigCpuTimer(&CpuTimer1, 200, 1*CPU_TIMER_MS);//1ms


    // Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM
}

void uint2hex(uint32_t data,char * pdata)
{
    int i;
    memset(pdata,'\0',sizeof(pdata));
    for (i = 0; i < 8 ; ++i) {
        pdata[7-i] = (data & 0x0000000F) + '0';
        data >>= 4;
    }
}

interrupt void ipc1_isr(void)
{
    // Read back ADC-A0 results and store in circular buffer
    cpu2_rev_cpu1_data = (Uint32) IpcRegs.IPCRECVDATA;
    uint2hex(cpu2_rev_cpu1_data, comm_data);
    scib_pdev->master->write_string(scib_pdev,"\t\t\t\t",-1);
    scib_pdev->master->write_string(scib_pdev,comm_data,-1);
    scib_pdev->master->write_string(scib_pdev,"\r\n",-1);
    // Return from interrupt
    IpcRegs.IPCACK.bit.IPC1 = 1;                // Clear IPC1 bit
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP1;     // Acknowledge PIE group 1 to enable further interrupts
}

void hex2int(unsigned char* ptrchar,uint32_t *ptrint)
{
    unsigned char i;
    uint32_t tmp_data=0;
    *ptrint = 0;// clear origin data
    for (i = 0; i < 8; ++i) {
        if (ptrchar[i]>='0'&&ptrchar[i]<='9') {         //'0' is 48
            tmp_data = (ptrchar[i]-48)<<(1*(7-i));
        } else if (ptrchar[i]>='A'&&ptrchar[i]<='F') {//'A' is 65
            tmp_data = (ptrchar[i]-55)<<(1*(7-i));
        }
        tmp_data = tmp_data << (3*(7-i));
        *ptrint |= tmp_data;
    }
}
void ipc_send_data(uint32_t data)
{
    IpcRegs.IPCSENDDATA = (Uint32) data;  // Write the result to the IPC data register
    IpcRegs.IPCSET.bit.IPC0 = 1;                // Set the IPC0 flag for CPU1
}

__interrupt void cpu_timer1_isr(void)
{
    StopCpuTimer1();
    ecap.ecap1_cap_counter = 0;
    ecap.ecap2_cap_counter = 0;
    ecap.ecap3_cap_counter = 0;

}

__interrupt void cpu_timer0_isr(void)
{
    //    StopCpuTimer0();

    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP1 | PIEACK_GROUP4| PIEACK_GROUP9;

    detect_program_begin();


    //    StartCpuTimer0();
}

inline void start_plause_channel_1(int num)
{
    int i;
    for (i = 0; i < num; ++i) {
        GPIO_WritePin(PLAUSE_CHANNEL_1, 1);
        DELAY_US((FREQ_PLAUSE_US + FREQ_PLAUSE_US_OFFSET));
        GPIO_WritePin(PLAUSE_CHANNEL_1, 0);
        DELAY_US(FREQ_PLAUSE_US);
    }
}

inline void start_plause_channel_2(int num)
{
    int i;
    for (i = 0; i < num; ++i) {
        GPIO_WritePin(PLAUSE_CHANNEL_2, 1);
        DELAY_US((FREQ_PLAUSE_US + FREQ_PLAUSE_US_OFFSET));
        GPIO_WritePin(PLAUSE_CHANNEL_2, 0);
        DELAY_US((FREQ_PLAUSE_US));
    }
}

inline void start_plause_channel_3(int num)
{
    int i;
    for (i = 0; i < num; ++i) {
        GPIO_WritePin(PLAUSE_CHANNEL_3, 1);
        DELAY_US((FREQ_PLAUSE_US + FREQ_PLAUSE_US_OFFSET));
        GPIO_WritePin(PLAUSE_CHANNEL_3, 0);
        DELAY_US((FREQ_PLAUSE_US));
    }
}

void detect_program_begin()
{
    // 声音在水中传播速度为1500m/s 相当于 1.5m/ms, 考虑到来回传播，若测量的1.5m的距离，需要2ms
    // 现需测得距离为10m，那么检测时间范围为: 0 - 13.3ms
    start_plause_channel_1(PLAUSE_NUMBER);
    DELAY_US(TIME_OFFSET_US);   // 跳过死区
    ecap1_start();              // 开启定时器1，排除干扰
    StartCpuTimer1();           //若定时器已经执行，再次执行该语句无效
    DELAY_MS( (4.0 * ecap.distance / 3.0) );
    ecap1_stop();

    //    ecap2_start();
    start_plause_channel_2(PLAUSE_NUMBER);
    DELAY_US(TIME_OFFSET_US);
    ecap2_start();
    StartCpuTimer1();   //若定时器已经执行，再次执行该语句无效
    DELAY_MS( (4.0 * ecap.distance / 3.0) );
    ecap2_stop();

    start_plause_channel_3(PLAUSE_NUMBER);
    DELAY_US(TIME_OFFSET_US);
    ecap3_start();
    StartCpuTimer1();   //若定时器已经执行，再次执行该语句无效
    DELAY_MS( (4.0 * ecap.distance / 3.0) );
    ecap3_stop();

    memset(ecap.ecap1_distance,0,sizeof(ecap.ecap1_distance));
    memset(ecap.ecap2_distance,0,sizeof(ecap.ecap2_distance));
    memset(ecap.ecap3_distance,0,sizeof(ecap.ecap3_distance));

    ecap.ecap1_time_us = ecap.ecap1_time_ns / 1000;
    ecap.ecap2_time_us = ecap.ecap2_time_ns / 1000;
    ecap.ecap3_time_us = ecap.ecap3_time_ns / 1000;
    if(ecap.ecap1_time_us > 0)
        ecap.ecap1_time_us += TIME_OFFSET_US; // 如果测得有回波，那么加上偏移时间
    if(ecap.ecap2_time_us > 0)
        ecap.ecap2_time_us += TIME_OFFSET_US; // 如果测得有回波，那么加上偏移时间
    if(ecap.ecap3_time_us > 0)
        ecap.ecap3_time_us += TIME_OFFSET_US; // 如果测得有回波，那么加上偏移时间

    ecap.ecap1_distance_float = (double)ecap.ecap1_time_us * 1.5 / 1000.0 / 2.0;
    ecap.ecap2_distance_float = (double)ecap.ecap2_time_us * 1.5 / 1000.0 / 2.0;
    ecap.ecap3_distance_float = (double)ecap.ecap3_time_us * 1.5 / 1000.0 / 2.0;

    ecap.ecap1_time_us = 0;
    ecap.ecap1_time_ns = 0;
    ecap.ecap2_time_us = 0;
    ecap.ecap2_time_ns = 0;
    ecap.ecap3_time_us = 0;
    ecap.ecap3_time_ns = 0;

    floatTochar(ecap.ecap1_distance_float,ecap.ecap1_distance,2);
    floatTochar(ecap.ecap2_distance_float,ecap.ecap2_distance,2);
    floatTochar(ecap.ecap3_distance_float,ecap.ecap3_distance,2);

    scib_pdev->master->write_string(scib_pdev,"distance: ",-1);
    scib_pdev->master->write_string(scib_pdev,ecap.ecap1_distance,-1);
    scib_pdev->master->write_string(scib_pdev,"m ",-1);
    scib_pdev->master->write_string(scib_pdev,ecap.ecap2_distance,-1);
    scib_pdev->master->write_string(scib_pdev,"m ",-1);
    scib_pdev->master->write_string(scib_pdev,ecap.ecap3_distance,-1);
    scib_pdev->master->write_string(scib_pdev,"m \r\n",-1);

}


