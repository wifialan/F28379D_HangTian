/*
 * adc.h
 *
 *  Created on: 2018Äê12ÔÂ1ÈÕ
 *      Author: multimicro
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_


#define     ADCAIN0         (1<<0)
#define     ADCAIN1         (1<<1)
#define     ADCAIN2         (1<<2)
#define     ADCAIN3         (1<<3)
#define     ADCAIN4         (1<<4)
#define     ADCAIN5         (1<<5)

#define RESULTS0_BUFFER_SIZE 800
#define RESULTS1_BUFFER_SIZE 1
#define RESULTS2_BUFFER_SIZE 1
#define RESULTS3_BUFFER_SIZE 1
#define RESULTS4_BUFFER_SIZE 1
#define RESULTS5_BUFFER_SIZE 1

void adc_init(uint16_t adc_channel);
void clear_buffer(void);
void SetupADCEpwm(uint16_t adc_channel);
void ConfigureEPWM(void);
void ConfigureADC(void);
interrupt void adca1_isr(void);

void stop_adc(void);
void start_adc(void);

#endif /* INC_ADC_H_ */
