/*
 * ecap.c
 *
 *  Created on: 2019��7��7��
 *      Author: multimicro
 */
#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2837xD_Ipc_drivers.h"
#include "ecap.h"
#include "stdlib.h"



ECAP_DEV ecap;

void init_ecap()
{

    //ecap = (ECAP_DEV *)malloc(sizoef(ECAP_DEV)*2);
    EALLOW;
    PieVectTable.ECAP1_INT = &ecap1_isr_signal;
    PieVectTable.ECAP2_INT = &ecap2_isr_signal;
    PieVectTable.ECAP3_INT = &ecap3_isr_signal;
    EDIS;

    InitEcapture1();
    InitEcapture2();
    InitEcapture3();

    ecap.distance = 10.0;

    ecap.ecap1_cap_counter = 0;
    ecap.ecap2_cap_counter = 0;
    ecap.ecap3_cap_counter = 0;

    ecap.ecap1_time_ns = 0;
    ecap.ecap2_time_ns = 0;
    ecap.ecap3_time_ns = 0;

    ecap.ecap1_time_us = 0;
    ecap.ecap2_time_us = 0;
    ecap.ecap3_time_us = 0;


}

void InitEcapture1()
{

    ECap1Regs.ECEINT.all = 0x0000;          // Disable all capture __interrupts
    ECap1Regs.ECCLR.all = 0xFFFF;           // Clear all CAP __interrupt flags
    ECap1Regs.ECCTL1.bit.CAPLDEN = 0;       // Disable CAP1-CAP4 register loads
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Make sure the counter is stopped


    /*
     *             _____       _____       _____       _____
     *            |     |     |     |     |     |     |     |
     *       _____|     |_____|     |_____|     |_____|     |_____...
     *            ��           ��           ��           ��
     *            A1          A2          A3          A4
     *
     */

    //
    // Configure peripheral registers
    //
    ECap1Regs.ECCTL1.bit.FREE_SOFT = 0;
    ECap1Regs.ECCTL2.bit.SWSYNC = 1;            //ͬ�����е�Ecapʱ��
    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;   // ����ģ�鹤��������ģʽ��
    ECap1Regs.ECCTL2.bit.STOP_WRAP = 3;     // (����ģʽ)Stop at 2 events��(����ģʽ)��CAP4�Ĳ�׽�¼��������������������
    ECap1Regs.ECCTL1.bit.CAP1POL = 0;       // Rising edge
    ECap1Regs.ECCTL1.bit.CAP2POL = 0;       // Rising edge
    ECap1Regs.ECCTL1.bit.CAP3POL = 0;       // Rising edge
    ECap1Regs.ECCTL1.bit.CAP4POL = 0;       // Rising edge
    ECap1Regs.ECCTL1.bit.CTRRST1 = 0;       // ��ɴ˴β�������ü�����
    ECap1Regs.ECCTL1.bit.CTRRST2 = 0;       // ��ɴ˴β�������ü�����
    ECap1Regs.ECCTL1.bit.CTRRST3 = 0;       // ��ɴ˴β�������ü�����
    ECap1Regs.ECCTL1.bit.CTRRST4 = 0;       // ��ɴ˴β�������ü�����
    ECap1Regs.ECCTL2.bit.SYNCI_EN = 1;      // Enable sync in
    ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0;     // Pass through
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units
    ECap1Regs.ECCTL2.bit.CAP_APWM = 0;          //������CAP����ģʽ

    //    ECap1Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable CAP1-CAP4 register loads

    //����һ��START�źź�����жϣ�һ��START�ź���һ�������غ��½��أ����������¼���ɺ��һ���ж�

    ECap1Regs.ECEINT.bit.CEVT1 = 1;         // 1 events = __interrupt
    ECap1Regs.ECEINT.bit.CEVT2 = 1;         // 2 events = __interrupt
    ECap1Regs.ECEINT.bit.CEVT3 = 1;         // 3 events = __interrupt
    ECap1Regs.ECEINT.bit.CEVT4 = 1;         // 4 events = __interrupt

    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Stop Counter
}

void InitEcapture2()
{

    ECap2Regs.ECEINT.all = 0x0000;          // Disable all capture __interrupts
    ECap2Regs.ECCLR.all = 0xFFFF;           // Clear all CAP __interrupt flags
    ECap2Regs.ECCTL1.bit.CAPLDEN = 0;       // Disable CAP1-CAP4 register loads
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Make sure the counter is stopped


    /*
     *             _____       _____       _____       _____
     *            |     |     |     |     |     |     |     |
     *       _____|     |_____|     |_____|     |_____|     |_____...
     *            ��           ��           ��           ��
     *            A1          A2          A3          A4
     *
     */

    //
    // Configure peripheral registers
    //
    ECap2Regs.ECCTL1.bit.FREE_SOFT = 0;
    ECap2Regs.ECCTL2.bit.SWSYNC = 1;            //ͬ�����е�Ecapʱ��
    ECap2Regs.ECCTL2.bit.CONT_ONESHT = 0;   // ����ģ�鹤��������ģʽ��
    ECap2Regs.ECCTL2.bit.STOP_WRAP = 3;     // (����ģʽ)Stop at 2 events��(����ģʽ)��CAP4�Ĳ�׽�¼��������������������
    ECap2Regs.ECCTL1.bit.CAP1POL = 0;       // Rising edge
    ECap2Regs.ECCTL1.bit.CAP2POL = 0;       // Rising edge
    ECap2Regs.ECCTL1.bit.CAP3POL = 0;       // Rising edge
    ECap2Regs.ECCTL1.bit.CAP4POL = 0;       // Rising edge
    ECap2Regs.ECCTL1.bit.CTRRST1 = 0;       // ��ɴ˴β�������ü�����
    ECap2Regs.ECCTL1.bit.CTRRST2 = 0;       // ��ɴ˴β�������ü�����
    ECap2Regs.ECCTL1.bit.CTRRST3 = 0;       // ��ɴ˴β�������ü�����
    ECap2Regs.ECCTL1.bit.CTRRST4 = 0;       // ��ɴ˴β�������ü�����
    ECap2Regs.ECCTL2.bit.SYNCI_EN = 1;      // Enable sync in
    ECap2Regs.ECCTL2.bit.SYNCO_SEL = 0;     // Pass through
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units
    ECap2Regs.ECCTL2.bit.CAP_APWM = 0;          //������CAP����ģʽ

    //    ECap2Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable CAP1-CAP4 register loads

    //����һ��START�źź�����жϣ�һ��START�ź���һ�������غ��½��أ����������¼���ɺ��һ���ж�

    ECap2Regs.ECEINT.bit.CEVT1 = 1;         // 1 events = __interrupt
    ECap2Regs.ECEINT.bit.CEVT2 = 1;         // 2 events = __interrupt
    ECap2Regs.ECEINT.bit.CEVT3 = 1;         // 3 events = __interrupt
    ECap2Regs.ECEINT.bit.CEVT4 = 1;         // 4 events = __interrupt

    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Stop Counter
}

void InitEcapture3()
{

    ECap3Regs.ECEINT.all = 0x0000;          // Disable all capture __interrupts
    ECap3Regs.ECCLR.all = 0xFFFF;           // Clear all CAP __interrupt flags
    ECap3Regs.ECCTL1.bit.CAPLDEN = 0;       // Disable CAP1-CAP4 register loads
    ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Make sure the counter is stopped


    /*
     *             _____       _____       _____       _____
     *            |     |     |     |     |     |     |     |
     *       _____|     |_____|     |_____|     |_____|     |_____...
     *            ��           ��           ��           ��
     *            A1          A2          A3          A4
     *
     */

    //
    // Configure peripheral registers
    //
    ECap3Regs.ECCTL1.bit.FREE_SOFT = 0;
    ECap3Regs.ECCTL2.bit.SWSYNC = 1;            //ͬ�����е�Ecapʱ��
    ECap3Regs.ECCTL2.bit.CONT_ONESHT = 0;   // ����ģ�鹤��������ģʽ��
    ECap3Regs.ECCTL2.bit.STOP_WRAP = 3;     // (����ģʽ)Stop at 2 events��(����ģʽ)��CAP4�Ĳ�׽�¼��������������������
    ECap3Regs.ECCTL1.bit.CAP1POL = 0;       // Rising edge
    ECap3Regs.ECCTL1.bit.CAP2POL = 0;       // Rising edge
    ECap3Regs.ECCTL1.bit.CAP3POL = 0;       // Rising edge
    ECap3Regs.ECCTL1.bit.CAP4POL = 0;       // Rising edge
    ECap3Regs.ECCTL1.bit.CTRRST1 = 0;       // ��ɴ˴β�������ü�����
    ECap3Regs.ECCTL1.bit.CTRRST2 = 0;       // ��ɴ˴β�������ü�����
    ECap3Regs.ECCTL1.bit.CTRRST3 = 0;       // ��ɴ˴β�������ü�����
    ECap3Regs.ECCTL1.bit.CTRRST4 = 0;       // ��ɴ˴β�������ü�����
    ECap3Regs.ECCTL2.bit.SYNCI_EN = 1;      // Enable sync in
    ECap3Regs.ECCTL2.bit.SYNCO_SEL = 0;     // Pass through
    ECap3Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units
    ECap3Regs.ECCTL2.bit.CAP_APWM = 0;          //������CAP����ģʽ

    //    ECap3Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
    ECap3Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable CAP1-CAP4 register loads

    //����һ��START�źź�����жϣ�һ��START�ź���һ�������غ��½��أ����������¼���ɺ��һ���ж�

    ECap3Regs.ECEINT.bit.CEVT1 = 1;         // 1 events = __interrupt
    ECap3Regs.ECEINT.bit.CEVT2 = 1;         // 2 events = __interrupt
    ECap3Regs.ECEINT.bit.CEVT3 = 1;         // 3 events = __interrupt
    ECap3Regs.ECEINT.bit.CEVT4 = 1;         // 4 events = __interrupt

    ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Stop Counter
}

__interrupt void ecap1_isr_signal(void)
{

    ReloadCpuTimer1();  //�����ÿִ��һ�Σ���ʱ��������һ��
    StartCpuTimer1();   //����ʱ���Ѿ�ִ�У��ٴ�ִ�и������Ч

    ecap.ecap1_cap_counter ++;

    if(ecap.ecap1_cap_counter >= 4)
    {
        StopCpuTimer1();
        ecap1_stop();
        ecap.ecap1_time_ns = ECap1Regs.TSCTR * 5;
        ecap.ecap1_time_us = ecap.ecap1_time_ns / 1000;
    }
    // ecap1_stop();
    // Open ecap1 uint
    // F28379D ��ʱ��Ƶ��Ϊ200MHz, ��û�з�Ƶ(�˴�û�з�Ƶ)������£�Ecapģ��ļ�ʱ����ÿ5ns����һ��

    ECap1Regs.ECCLR.bit.CEVT1 = 1;
    ECap1Regs.ECCLR.bit.CEVT2 = 1;
    ECap1Regs.ECCLR.bit.CEVT3 = 1;
    ECap1Regs.ECCLR.bit.CEVT4 = 1;
    ECap1Regs.ECCLR.bit.INT = 1;
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;   //ʹ���ڲ����¼��м���CAP1-4�Ĵ����¼�
    //    ECap1Regs.ECCTL2.bit.REARM = 1;
    //
    // Acknowledge this __interrupt to receive more __interrupts from group 4
    //
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP4;
}

__interrupt void ecap2_isr_signal(void)
{
    ReloadCpuTimer1();  //�����ÿִ��һ�Σ���ʱ��������һ��
    StartCpuTimer1();   //����ʱ���Ѿ�ִ�У��ٴ�ִ�и������Ч

    ecap.ecap2_cap_counter ++;

    if(ecap.ecap2_cap_counter >= 4)
    {
        StopCpuTimer1();
        ecap2_stop();
        ecap.ecap2_time_ns = ECap2Regs.TSCTR * 5;
        ecap.ecap2_time_us = ecap.ecap2_time_ns / 1000;
    }
    // ecap2_stop();
    // Open ecap2 uint
    // F28379D ��ʱ��Ƶ��Ϊ200MHz, ��û�з�Ƶ(�˴�û�з�Ƶ)������£�Ecapģ��ļ�ʱ����ÿ5ns����һ��

    ECap2Regs.ECCLR.bit.CEVT1 = 1;
    ECap2Regs.ECCLR.bit.CEVT2 = 1;
    ECap2Regs.ECCLR.bit.CEVT3 = 1;
    ECap2Regs.ECCLR.bit.CEVT4 = 1;
    ECap2Regs.ECCLR.bit.INT = 1;
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;   //ʹ���ڲ����¼��м���CAP1-4�Ĵ����¼�
    //    ECap2Regs.ECCTL2.bit.REARM = 1;   //Reopen Ecap unit
    //
    // Acknowledge this __interrupt to receive more __interrupts from group 4
    //
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP4;
}

__interrupt void ecap3_isr_signal(void)
{
    ReloadCpuTimer1();  //�����ÿִ��һ�Σ���ʱ��������һ��
    StartCpuTimer1();   //����ʱ���Ѿ�ִ�У��ٴ�ִ�и������Ч

    ecap.ecap3_cap_counter ++;

    if(ecap.ecap3_cap_counter >= 4)
    {
        StopCpuTimer1();
        ecap3_stop();
        ecap.ecap3_time_ns = ECap3Regs.TSCTR * 5;
        ecap.ecap3_time_us = ecap.ecap3_time_ns / 1000;
    }
    // ecap3_stop();
    // Open ecap3 uint
    // F28379D ��ʱ��Ƶ��Ϊ200MHz, ��û�з�Ƶ(�˴�û�з�Ƶ)������£�Ecapģ��ļ�ʱ����ÿ5ns����һ��

    ECap3Regs.ECCLR.bit.CEVT1 = 1;
    ECap3Regs.ECCLR.bit.CEVT2 = 1;
    ECap3Regs.ECCLR.bit.CEVT3 = 1;
    ECap3Regs.ECCLR.bit.CEVT4 = 1;
    ECap3Regs.ECCLR.bit.INT = 1;
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;   //ʹ���ڲ����¼��м���CAP1-4�Ĵ����¼�
    //    ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0; //Freeze counter
    //    ECap2Regs.ECCTL2.bit.REARM = 1;   //Reopen Ecap unit
    //
    // Acknowledge this __interrupt to receive more __interrupts from group 4
    //
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP4;
}

void ecap1_start()
{
    ecap.ecap1_cap_counter = 0;
    ecap.ecap1_time_us = 0;
    ecap.ecap1_time_ns = 0;
    ecap.ecap1_cap_counter = 0;
    ECap1Regs.TSCTR = 0;
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
    //    ECap1Regs.ECCTL2.bit.REARM = 1;
}

void ecap2_start()
{
    ecap.ecap2_cap_counter = 0;
    ecap.ecap2_time_us = 0;
    ecap.ecap2_time_ns = 0;
    ECap2Regs.TSCTR = 0;
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
    //    ECap2Regs.ECCTL2.bit.REARM = 1;
}

void ecap3_start()
{
    ecap.ecap3_cap_counter = 0;
    ecap.ecap3_time_us = 0;
    ecap.ecap3_time_ns = 0;
    ECap3Regs.TSCTR = 0;
    ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
    //    ECap3Regs.ECCTL2.bit.REARM = 1;
}

void ecap1_stop()
{
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Stop Counter
    //    ECap1Regs.ECCTL2.bit.REARM = 0;
}
void ecap2_stop()
{
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Stop Counter
    //    ECap2Regs.ECCTL2.bit.REARM = 0;
}
void ecap3_stop()
{
    ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Stop Counter
    //    ECap3Regs.ECCTL2.bit.REARM = 0;
}
