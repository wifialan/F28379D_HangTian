/*
 * adc.c
 *
 *  Created on: 2018锟斤拷12锟斤拷1锟斤拷
 *      Author: multimicro
 */
#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "adc.h"

int Adca0Results[RESULTS0_BUFFER_SIZE];
//Uint16 Adca1Results[RESULTS1_BUFFER_SIZE];
//Uint16 Adca2Results[RESULTS2_BUFFER_SIZE];
//Uint16 Adca3Results[RESULTS3_BUFFER_SIZE];
//Uint16 Adca4Results[RESULTS4_BUFFER_SIZE];
//Uint16 Adca5Results[RESULTS5_BUFFER_SIZE];

Uint16 results0Index;//,results1Index,results2Index, \
results3Index,results4Index,results5Index;
//buffer for storing conversion results

void adc_init(uint16_t adc_channel)
{

    //Map ISR functions
    EALLOW;
    PieVectTable.ADCA1_INT = &adca1_isr; //function for ADCA interrupt 1
    EDIS;
    //Configure the ADC and power it up
    ConfigureADC();

    //Configure the ePWM
    ConfigureEPWM();

    //Setup the ADC for ePWM triggered conversions on channel 0
    SetupADCEpwm(adc_channel);

    //Enable global Interrupts and higher priority real-time debug events:
    IER |= M_INT1; //Enable group 1 interrupts
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM
    //Initialize results buffer
    clear_buffer();

    //enable PIE interrupt
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;

    //sync ePWM
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

}

void clear_buffer(void)
{
}

//Write ADC configurations and power up the ADC for both ADC A and ADC B
void ConfigureADC(void)
{
    EALLOW;

    //write configurations
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    //Set pulse positions to late
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;

    //power up the ADC
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;

    //delay for 1ms to allow ADC time to power up
    DELAY_US(1000);

    EDIS;
}

void ConfigureEPWM(void)
{
    EALLOW;
    // Assumes ePWM clock is already enabled
    EPwm1Regs.ETSEL.bit.SOCAEN  = 0;            // Disable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = 4;            // Select SOC on up-count
    EPwm1Regs.ETPS.bit.SOCAPRD = 1;             // Generate pulse on 1st event
    EPwm1Regs.CMPA.bit.CMPA = 0x0000;           // The value must smaller than TBPRD value
    /*
     * 499 - 100khz
     * 249 - 200khz
     * 142 - 350khz
     * 124 - 400khz
     * */
    EPwm1Regs.TBPRD = 499;// 100khz             // set pwm frequency Tpwm=(TBPRD+1)*Ttbclk// Ftbclk = 50MHz
    EPwm1Regs.TBCTL.bit.CTRMODE = 3;            // freeze counter
    EDIS;
}

void SetupADCEpwm(uint16_t adc_channel)
{
    Uint16 acqps;

    //determine minimum acquisition window (in SYSCLKS) based on resolution
    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION){
        acqps = 14; //75ns
    }
    else { //resolution is 16-bit
        acqps = 63; //320ns
    }

    //Select the channels to convert and end of conversion flag
    //A0-A5 六通道可同步采样
    //同组ADC(如ADCA组)通道下一个SOC(Start-of-Conversion)信号只能转换一个通道
    //不同组(如ADCA和ADCD两组)下可用同一个SOC转换不同组下的一个AD通道
    //F28379D共有16个通道，因此最多有16个SOC信号
    //一个触发信号可以使能同组(如同为ADCSOCA或ADCSOCB组)下多个SOC信号
    //本程序采用Epwm触发ADC转换采用，最大可采用六通道
    EALLOW;

    // ADCA0
    if(adc_channel & ADCAIN0) {
        AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC0 will convert pin A0
        AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
        AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA/C
    }
    // ADCA1
    if(adc_channel & ADCAIN1) {
        AdcaRegs.ADCSOC1CTL.bit.CHSEL = 1;  //SOC1 will convert pin A1
        AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
        AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA/C
    }
    // ADCA2
    if(adc_channel & ADCAIN2) {
        AdcaRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC2 will convert pin A2
        AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
        AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA/C
    }
    // ADCA3
    if(adc_channel & ADCAIN3) {
        AdcaRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC3 will convert pin A3
        AdcaRegs.ADCSOC3CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
        AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA/C
    }
    // ADCA4
    if(adc_channel & ADCAIN4) {
        AdcaRegs.ADCSOC4CTL.bit.CHSEL = 4;  //SOC4 will convert pin A4
        AdcaRegs.ADCSOC4CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
        AdcaRegs.ADCSOC4CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA/C
    }
    // ADCA5
    if(adc_channel & ADCAIN5) {
        AdcaRegs.ADCSOC5CTL.bit.CHSEL = 5;  //SOC5 will convert pin A5
        AdcaRegs.ADCSOC5CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
        AdcaRegs.ADCSOC5CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA/C
    }

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; //end of SOC0 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
}

interrupt void adca1_isr(void)
{
    while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0);

    Adca0Results[results0Index] = AdcaResultRegs.ADCRESULT0;
//    Adca1Results[results1Index] = AdcaResultRegs.ADCRESULT1;
//    Adca2Results[results2Index] = AdcaResultRegs.ADCRESULT2;
//    Adca3Results[results3Index] = AdcaResultRegs.ADCRESULT3;
//    Adca4Results[results4Index] = AdcaResultRegs.ADCRESULT4;
//    Adca5Results[results5Index] = AdcaResultRegs.ADCRESULT5;

    results0Index = ++results0Index % RESULTS0_BUFFER_SIZE;
//    results1Index = ++results1Index % RESULTS1_BUFFER_SIZE;
//    results2Index = ++results2Index % RESULTS2_BUFFER_SIZE;
//    results3Index = ++results3Index % RESULTS2_BUFFER_SIZE;
//    results4Index = ++results4Index % RESULTS4_BUFFER_SIZE;
//    results5Index = ++results5Index % RESULTS5_BUFFER_SIZE;

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP1;
}

void start_adc(void)
{
    //start ePWM
    EALLOW;
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;  //enable SOCA
    EPwm1Regs.TBCTL.bit.CTRMODE = 0; //unfreeze, and enter up count mode
    EDIS;
}

void stop_adc(void)
{
    //stop ePWM
    EALLOW;
    EPwm1Regs.ETSEL.bit.SOCAEN = 0;  //disable SOCA
    EPwm1Regs.TBCTL.bit.CTRMODE = 3; //freeze counter
    EDIS;
}
