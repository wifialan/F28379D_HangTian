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
#include <stdio.h>
#include <stdlib.h>
#include "project.h"
//#include "adc.h"
//#include "demod_fsk.h"
//#include "cfft_icfft.h"
#include "math.h"
//#include "data.h"

#define TEST

void hex2int(unsigned char* ptrchar,uint32_t *ptrint);
void init_avoid_abstacle_plause();

AD9833 ad9833_dev, *ad9833_dev_handle;
ESCI *scib_pdev;


struct sys_status sys;
uint32_t cpu1_rev_cpu2_data;

extern int Adca0Results[RESULTS0_BUFFER_SIZE];
extern Uint16 results0Index;

void main(void)
{
    f28379d_system_init();

//    GPIO_SetupPinOptions(81, GPIO_OUTPUT, GPIO_PUSHPULL);
//    GPIO_SetupPinMux(81, GPIO_MUX_CPU1, 0);
//    GPIO_SetupPinOptions(82, GPIO_OUTPUT, GPIO_PUSHPULL);
//    GPIO_SetupPinMux(82, GPIO_MUX_CPU1, 0);
//    GPIO_WritePin(82, 1);
//    GPIO_WritePin(82, 0);
//    GPIO_WritePin(82, 1);

#ifdef _STANDALONE
    // Wait here until CPU02 is ready
    while (IpcRegs.IPCSTS.bit.IPC7 == 0); // Wait for CPU02 to set IPC07
    IpcRegs.IPCACK.bit.IPC7 = 1;           // Acknowledge and clear IPC07
#endif
    //init_sci();

    //Setting system default status
    sys.send_or_rece_state = STATUS_RECE;
    sys.demod_state = STATUE_DEMOD_STOP;

    ad9833_dev_handle       = &ad9833_dev;
#ifndef CPU2_CTRL_UARTB
    scib_pdev = sci_new_dev(SCIB, 115200);
#endif

    ad9833_init(ad9833_dev_handle,SPIA);
    ad9833_dev_handle->set_wave_para(ad9833_dev_handle, 10ul*KHz, AD9833_REG_FREQ0,0, SQU);
    ad9833_dev_handle->reset(ad9833_dev_handle);

    //    StopCpuTimer0();
    //    ConfigCpuTimer(&CpuTimer0, 200, 100000);//0.1s
    //    StartCpuTimer0();
    DELAY_MS(100);
#ifndef CPU2_CTRL_UARTB
    scib_pdev->master->write_string(scib_pdev,"Running\r\n",-1);
#endif
    memset(Adca0Results, 0, sizeof(Adca0Results));
    start_adc();
    DELAY_US(10);
#ifndef TEST
    while(1){
        while(Adca0Results[results0Index]<3000);
        while(results0Index<(RESULTS0_BUFFER_SIZE-10));
        stop_adc();
        memset(Adca0Results, 0, sizeof(Adca0Results));
        results0Index=0;
    }
#endif
    while(1)
    {
        if (sys.send_or_rece_state == STATUS_SEND) {
            stop_adc();
            qfsk_send_data(cpu1_rev_cpu2_data);
            //            ipc_send_data((uint32_t)QFSK_DATA_SEND_SUCCEEDED);
            sys.send_or_rece_state = STATUS_RECE;//若没有接收到串口发送过来的数据，则停止发送
            start_adc();
        } else if (sys.send_or_rece_state == STATUS_RECE){
#ifndef CPU2_CTRL_UARTB
            scib_pdev->master->write_string(scib_pdev,"\n-----------------------------\n",-1);
            scib_pdev->master->write_string(scib_pdev,"Begin to demod QFSK data\n",-1);
            scib_pdev->master->write_string(scib_pdev,"-----------------------------\n",-1);
#endif
            demod_fsk();
        }
    }
}

void f28379d_system_init(void)
{
    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the F2837xD_SysCtrl.c file.
    InitSysCtrl();

#ifdef _STANDALONE
#ifdef _FLASH
    // Send boot command to allow the CPU2 application to begin execution
    IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH);
#else
    // Send boot command to allow the CPU2 application to begin execution
    IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_RAM);
#endif
#endif

    // Step 2. Initialize GPIO:
    // This example function is found in the F2837xD_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    InitGpio(); // Skipped for this example

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

    init_sci();

    init_ecap();

    init_avoid_abstacle_plause();
    EALLOW;  // This is needed to write to EALLOW protected registers
    //    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    PieVectTable.IPC0_INT = &ipc0_isr;      // IPC0 interrupt
#ifndef CPU2_CTRL_UARTB
    PieVectTable.SCIB_RX_INT = &scibRxFifoIsr;
#endif
    //    PieVectTable.SCIA_RX_INT = &sciaRxFifoIsr;
    //    PieVectTable.SCIB_RX_INT = &scibRxFifoIsr;
    //    PieVectTable.SCIC_RX_INT = &scicRxFifoIsr;
    //    PieVectTable.SCID_RX_INT = &scidRxFifoIsr;
    EDIS;   // This is needed to disable write to EALLOW protected registers

    IpcRegs.IPCCLR.all = 0xFFFFFFFF;

    //    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
    //    PieCtrlRegs.PIEIER9.bit.INTx1=1;     // PIE Group 9, INT1
    //    PieCtrlRegs.PIEIER9.bit.INTx2=0;     // PIE Group 9, INT2
    //PieCtrlRegs.PIEIER1.bit.INTx7 = 1;         // Enable TINT0 in the PIE: Group 1 __interrupt 7
    PieCtrlRegs.PIEIER1.bit.INTx13 = 1;     // IPC0 ISR
#ifndef CPU2_CTRL_UARTB
    PieCtrlRegs.PIEIER9.bit.INTx3=1;     // PIE Group 9, INT3, SCIB_RX.
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1 | PIEACK_GROUP9;
    IER |= M_INT1 | M_INT9; // Enable CPU INT
#else
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    IER |= M_INT1; // Enable CPU INT
#endif
    //    InitCpuTimers();
    //    ConfigCpuTimer(&CpuTimer0, 200, 10000);//10ms
    //ADC Init
    adc_init(ADCAIN0+ADCAIN1+ADCAIN2+ADCAIN3+ADCAIN4+ADCAIN5);

    //init_pwm();

    // Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    EALLOW;
    //    GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 0;
    //    GpioCtrlRegs.GPBDIR.bit.GPIO37 = 1;
    //TODO Add code to allow configuration of GPADIR from CPU02 using IPC
    EDIS;

    //    CpuTimer0Regs.TCR.bit.TSS = 0;//start timer0
    //StartCpuTimer0();
//    DELAY_MS(1000);

}

void init_avoid_abstacle_plause()
{
    GPIO_SetupPinOptions(PLAUSE_CHANNEL_1, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(PLAUSE_CHANNEL_1, GPIO_MUX_CPU2, 0);
    GPIO_SetupPinOptions(PLAUSE_CHANNEL_2, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(PLAUSE_CHANNEL_2, GPIO_MUX_CPU2, 0);
    GPIO_SetupPinOptions(PLAUSE_CHANNEL_3, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(PLAUSE_CHANNEL_3, GPIO_MUX_CPU2, 0);
}

void ipc_send_data(uint32_t data)
{
    IpcRegs.IPCSENDDATA = (Uint32) data;  // Write the result to the IPC data register
    IpcRegs.IPCSET.bit.IPC1 = 1;                // Set the IPC1 flag for CPU2
}

__interrupt void cpu_timer0_isr(void)
{
    //    CpuTimer0.InterruptCount++;

    CpuTimer0.InterruptCount = ( ++CpuTimer0.InterruptCount ) % 1000000;

    if (CpuTimer0.InterruptCount % 2 == 0) {
        GPIO_WritePin(37, 0);
    } else {
        GPIO_WritePin(37, 1);
    }
    //
    // Acknowledge this __interrupt to receive more __interrupts from group 1
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1 | PIEACK_GROUP9;
}

interrupt void ipc0_isr(void)
{

    // Read IPC channel 0
    cpu1_rev_cpu2_data = (Uint32) IpcRegs.IPCRECVDATA;  // Read data on IPC ADDR register
    // Return from interrupt
    IpcRegs.IPCACK.bit.IPC0 = 1;                // Clear IPC0 bit
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP1;     // Acknowledge PIE group 1 to enable further interrupts

    sys.send_or_rece_state = STATUS_SEND;
}


