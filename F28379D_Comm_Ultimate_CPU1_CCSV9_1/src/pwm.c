/*
 * pwm.c
 *
 *  Created on: 2019Äê12ÔÂ3ÈÕ
 *      Author: multimicro
 */



#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "pwm.h"

#define CPU2_CTRL_EPWM


void init_pwm(void)
{
    InitEPwm1Gpio();
    InitEPwm2Gpio();
    InitEPwm3Gpio();
    InitEPwm4Gpio();

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC =0;
    EDIS;
#ifdef CPU2_CTRL_EPWM
    EALLOW;
    //Transfer ownership between CPU01 and CPU02, for detail, see Technical Reference Manual(spruhm8h) page 305
    DevCfgRegs.CPUSEL0.bit.EPWM1 = 1;// Transfer ownership of EPWM1 to CPU02
    DevCfgRegs.CPUSEL0.bit.EPWM2 = 1;// Transfer ownership of EPWM2 to CPU02
    DevCfgRegs.CPUSEL0.bit.EPWM3 = 1;// Transfer ownership of EPWM3 to CPU02
    DevCfgRegs.CPUSEL0.bit.EPWM4 = 1;// Transfer ownership of EPWM4 to CPU02
    EDIS;
#else
    InitEPwm1Example();
    init_pwm_pulse();
#endif
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC =1;
    EDIS;
}
#ifndef CPU2_CTRL_EPWM
void init_pwm_pulse()
{
    init_pwm_2_pulse();
    init_pwm_3_pulse();
    init_pwm_4_pulse();

}

void init_pwm_2_pulse()
{
    // Set timer period
    EPwm2Regs.TBPRD = 500;//50MHz clock rate is 100KHz PWM  50MHz/100KHz = 500
    EPwm2Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm2Regs.TBCTR = 0x0000;                     // Clear counter

    //
    // Setup TBCLK
    //
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_FREEZE; // Count up
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;       // Clock ratio to SYSCLKOUT(200MHz)
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    //Final clock freq is 100MHz(but the real is 50MHz)

    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;

    //
    // Setup compare
    //
    EPwm2Regs.CMPA.bit.CMPA = (EPwm2Regs.TBPRD/2);

    //
    // Set actions
    //
    EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
    EPwm2Regs.AQCTLA.bit.PRD = AQ_CLEAR;

    EPwm2Regs.AQCTLB.bit.CAU = AQ_CLEAR;          // Set PWM1A on Zero
    EPwm2Regs.AQCTLB.bit.CAD = AQ_SET;

    EPwm2Regs.ETSEL.bit.INTEN = 0;               // Disable INT
}

void init_pwm_3_pulse()
{
    // Set timer period
    EPwm3Regs.TBPRD = 500;//50MHz clock rate is 100KHz PWM  50MHz/100KHz = 500
    EPwm3Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm3Regs.TBCTR = 0x0000;                     // Clear counter

    //
    // Setup TBCLK
    //
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_FREEZE; // Count up
    EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;       // Clock ratio to SYSCLKOUT(200MHz)
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    //Final clock freq is 100MHz(but the real is 50MHz)

    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
    EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;
    EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;

    //
    // Setup compare
    //
    EPwm3Regs.CMPA.bit.CMPA = (EPwm3Regs.TBPRD/2);

    //
    // Set actions
    //
    EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
    EPwm3Regs.AQCTLA.bit.PRD = AQ_CLEAR;

    EPwm3Regs.AQCTLB.bit.CAU = AQ_CLEAR;          // Set PWM1A on Zero
    EPwm3Regs.AQCTLB.bit.CAD = AQ_SET;

    EPwm3Regs.ETSEL.bit.INTEN = 0;               // Disable INT
}

void init_pwm_4_pulse()
{
    // Set timer period
    EPwm4Regs.TBPRD = 500;//50MHz clock rate is 100KHz PWM  50MHz/100KHz = 500
    EPwm4Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm4Regs.TBCTR = 0x0000;                     // Clear counter

    //
    // Setup TBCLK
    //
    EPwm4Regs.TBCTL.bit.CTRMODE = TB_FREEZE; // Count up
    EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;       // Clock ratio to SYSCLKOUT(200MHz)
    EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    //Final clock freq is 100MHz(but the real is 50MHz)

    EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
    EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;
    EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;

    //
    // Setup compare
    //
    EPwm4Regs.CMPA.bit.CMPA = (EPwm4Regs.TBPRD/2);

    //
    // Set actions
    //
    EPwm4Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
    EPwm4Regs.AQCTLA.bit.PRD = AQ_CLEAR;

    EPwm4Regs.AQCTLB.bit.CAU = AQ_CLEAR;          // Set PWM1A on Zero
    EPwm4Regs.AQCTLB.bit.CAD = AQ_SET;

    EPwm4Regs.ETSEL.bit.INTEN = 0;               // Disable INT
}

void start_pwm_2_pulse()
{
    EPwm2Regs.TBCTR = 0x0000;                     // Clear counter
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // start pwm. mode:Count up
    DELAY_US(82);// 100KHz pulse
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_FREEZE; // stop pwm.
}
void start_pwm_3_pulse()
{
    EPwm3Regs.TBCTR = 0x0000;                     // Clear counter
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // start pwm. mode:Count up
    DELAY_US(82);
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_FREEZE; // stop pwm.
}
void start_pwm_4_pulse()
{
    EPwm4Regs.TBCTR = 0x0000;                     // Clear counter
    EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // start pwm. mode:Count up
    DELAY_US(82);
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_FREEZE; // stop pwm.
}

void InitEPwm1Example()
{
    // Set timer period
    EPwm1Regs.TBPRD = 2630;//50MHz clock rate is 19KHz PWM
    EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm1Regs.TBCTR = 0x0000;                     // Clear counter

    //
    // Setup TBCLK
    //
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;       // Clock ratio to SYSCLKOUT(200MHz)
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    //Final clock freq is 100MHz(but the real is 50MHz)

    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;

    //
    // Setup compare
    //
    EPwm1Regs.CMPA.bit.CMPA = (EPwm1Regs.TBPRD/2);

    //
    // Set actions
    //
    EPwm1Regs.AQCTLA.bit.CAU = AQ_TOGGLE;            // Set PWM1A on Zero
    EPwm1Regs.AQCTLA.bit.PRD = AQ_TOGGLE;

    EPwm1Regs.AQCTLB.bit.CAU = AQ_CLEAR;          // Set PWM1A on Zero
    EPwm1Regs.AQCTLB.bit.CAD = AQ_SET;

    //
    // Active Low PWMs - Setup Deadband
    //
    //    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    //    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_LO;
    //    EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    //    EPwm1Regs.DBRED.bit.DBRED = 0;
    //    EPwm1Regs.DBFED.bit.DBFED = 0;

    //
    // Interrupt where we will change the Deadband
    //
    EPwm1Regs.ETSEL.bit.INTEN = 0;               // Disable INT
}
#endif
