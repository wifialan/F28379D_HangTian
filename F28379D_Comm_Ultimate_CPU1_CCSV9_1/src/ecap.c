/*
 * ecap.c
 *
 *  Created on: 2019��7��7��
 *      Author: multimicro
 */
#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2837xD_Ipc_drivers.h"
#include "ecap.h"

#define CPU2_CTRL_ECAP

#ifndef CPU2_CTRL_ECAP
uint32_t cap1_counter_start;
uint32_t cap2_counter_stop;
uint32_t tof_time_ns;
uint32_t tof_start_ns;
uint32_t tdc_detect_time;
uint16_t ecap_flag=0;
#endif

void init_ecap()
{

    //    InitECap1Gpio(74);//74ָ����GPIO74���ú�����F2837xD_ECap.c����
    //
    //    GPIO_SetupPinOptions(74, GPIO_INPUT, GPIO_ASYNC);

    InitECap1Gpio(ECAP_PLAUSE_CHANNEL_1);
    GPIO_SetupPinOptions(ECAP_PLAUSE_CHANNEL_1, GPIO_INPUT, GPIO_ASYNC);

    InitECap2Gpio(ECAP_PLAUSE_CHANNEL_2);
    GPIO_SetupPinOptions(ECAP_PLAUSE_CHANNEL_2, GPIO_INPUT, GPIO_ASYNC);

    InitECap3Gpio(ECAP_PLAUSE_CHANNEL_3);
    GPIO_SetupPinOptions(ECAP_PLAUSE_CHANNEL_3, GPIO_INPUT, GPIO_ASYNC);

#ifdef CPU2_CTRL_ECAP
    EALLOW;
    DevCfgRegs.CPUSEL1.bit.ECAP1 = 1;// Transfer ownership of ECAP1 to CPU02
    DevCfgRegs.CPUSEL1.bit.ECAP2 = 1;// Transfer ownership of ECAP1 to CPU02
    DevCfgRegs.CPUSEL1.bit.ECAP3 = 1;// Transfer ownership of ECAP1 to CPU02
    EDIS;
#else
    InitEcapture1();
    InitEcapture2();
#endif
}

#ifndef CPU2_CTRL_ECAP
void InitEcapture1()
{

    ECap1Regs.ECEINT.all = 0x0000;          // Disable all capture __interrupts
    ECap1Regs.ECCLR.all = 0xFFFF;           // Clear all CAP __interrupt flags
    ECap1Regs.ECCTL1.bit.CAPLDEN = 0;       // Disable CAP1-CAP4 register loads
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Make sure the counter is stopped

    /*  START�ź�
     *          ___________
     *         |           |
     *    _____|           |____________________________
     *         ��           ��
     *         A1          B1
     *
     * */

    //
    // Configure peripheral registers
    //
    ECap1Regs.ECCTL2.bit.SWSYNC = 1;            //ͬ�����е�Ecapʱ��
    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 1;   // One-shot
    ECap1Regs.ECCTL2.bit.STOP_WRAP = 1;     // Stop at 2 events
    ECap1Regs.ECCTL1.bit.CAP1POL = 0;       // Rising edge
    ECap1Regs.ECCTL1.bit.CAP2POL = 1;       // Falling edge
    //    ECap1Regs.ECCTL1.bit.CAP3POL = 1;       // Falling edge
    //    ECap1Regs.ECCTL1.bit.CAP4POL = 0;       // Rising edge
    ECap1Regs.ECCTL1.bit.CTRRST1 = 0;       // ��ɴ˴β�������ü�����
    ECap1Regs.ECCTL1.bit.CTRRST2 = 1;       // ��ɴ˴β�������ü�����
    //    ECap1Regs.ECCTL1.bit.CTRRST3 = 1;       // Difference operation
    //    ECap1Regs.ECCTL1.bit.CTRRST4 = 1;       // Difference operation
    ECap1Regs.ECCTL2.bit.SYNCI_EN = 1;      // Enable sync in
    ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0;     // Pass through
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units
    ECap1Regs.ECCTL2.bit.CAP_APWM = 0;          //������CAP����ģʽ

    ECap1Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable CAP1-CAP4 register loads

    //����һ��START�źź�����жϣ�һ��START�ź���һ�������غ��½��أ����������¼���ɺ��һ���ж�
    ECap1Regs.ECEINT.bit.CEVT2 = 1;         // 2 events = __interrupt
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
}

void InitEcapture2()
{

    ECap2Regs.ECEINT.all = 0x0000;          // Disable all capture __interrupts
    ECap2Regs.ECCLR.all = 0xFFFF;           // Clear all CAP __interrupt flags
    ECap2Regs.ECCTL1.bit.CAPLDEN = 0;       // Disable CAP1-CAP4 register loads
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Make sure the counter is stopped

    /*  STOP�ź�
     *             _____       _____
     *            |     |     |     |
     *       _____|     |_____|     |__________...
     *            ��     ��     ��     ��
     *            A1    B1    C1    D1
     *
     */

    //
    // Configure peripheral registers
    //
    ECap2Regs.ECCTL2.bit.SWSYNC = 1;            //ͬ�����е�Ecapʱ��
    ECap2Regs.ECCTL2.bit.CONT_ONESHT = 1;   // arm one-shotģʽ
    ECap2Regs.ECCTL2.bit.STOP_WRAP = 1;     // Stop at 2 events
    ECap2Regs.ECCTL1.bit.CAP1POL = 0;       // Rising edge
    ECap2Regs.ECCTL1.bit.CAP2POL = 1;       // Falling edge
    ECap2Regs.ECCTL1.bit.CTRRST1 = 0;       // ��ɴ˴β�������ü�����
    ECap2Regs.ECCTL1.bit.CTRRST2 = 1;       // ��ɴ˴β�������ü�����
    //    ECap2Regs.ECCTL1.bit.CTRRST3 = 0;       // ��ɴ˴β�������ü�����
    //    ECap2Regs.ECCTL1.bit.CTRRST4 = 1;       // ��ɴ˴β�������ü�����
    ECap2Regs.ECCTL2.bit.SYNCI_EN = 1;      // Enable sync in
    ECap2Regs.ECCTL2.bit.SYNCO_SEL = 0;     // Pass through
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units

    //    ECap2Regs.ECCTL2.bit.CAP_APWM = 0;          //������CAP����ģʽ
    //    ECap2Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable CAP1-CAP4 register loads

    //����һ��START�źź�����жϣ�һ��START�ź���һ�������غ��½��أ����������¼���ɺ��һ���ж�
    ECap2Regs.ECEINT.bit.CEVT2 = 1;         // 2 events = __interrupt
    //        ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Open Counter in ecap1 interrupt function
}

__interrupt void ecap1_isr_start_signal(void)
{

    //Open ecap1 uint
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
    ECap2Regs.ECCTL2.bit.REARM = 1;

    tof_start_ns = ECap1Regs.CAP2 - ECap1Regs.CAP1;

    ECap1Regs.ECCLR.bit.CEVT2 = 1;
    ECap1Regs.ECCLR.bit.INT = 1;
    ECap1Regs.ECCTL2.bit.REARM = 1;
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;   //ʹ���ڲ����¼��м���CAP1-4�Ĵ����¼�
    //
    // Acknowledge this __interrupt to receive more __interrupts from group 4
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

__interrupt void ecap2_isr_stop_signal(void)
{
    //F28379Dʱ��Ƶ��Ϊ200MHz��������ÿ5ns����1
    //    cap1_counter_start = ECap1Regs.CAP1;
    tof_time_ns  = ECap2Regs.CAP1 ;//+ tof_start_ns;
    tdc_detect_time = tof_time_ns * 5 / 1000;//��λΪus

    ECap2Regs.ECCLR.bit.CEVT2 = 1;//�����4�¼��жϱ�־λ
    ECap2Regs.ECCLR.bit.INT = 1;
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;   //ʹ���ڲ����¼��м���CAP1-4�Ĵ����¼�
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0; //Freeze counter
    //    ECap2Regs.ECCTL2.bit.REARM = 1;   //Reopen Ecap unit
    //
    // Acknowledge this __interrupt to receive more __interrupts from group 4
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}
#endif
