/*
 * ecap.h
 *
 *  Created on: 2019Äê7ÔÂ7ÈÕ
 *      Author: multimicro
 */

#ifndef INC_ECAP_H_
#define INC_ECAP_H_

#define START_PLAUSE        74
#define STOP_PLAUSE         75

#define     PLAUSE_CHANNEL_1        28
#define     PLAUSE_CHANNEL_2        29
#define     PLAUSE_CHANNEL_3        31

#define     ECAP_PLAUSE_CHANNEL_1   35
#define     ECAP_PLAUSE_CHANNEL_2   36
#define     ECAP_PLAUSE_CHANNEL_3   37




void init_ecap();
void InitEcapture1();
void InitEcapture2();
void InitEcapture3();
__interrupt void ecap1_isr_signal(void);
__interrupt void ecap2_isr_signal(void);


#endif /* INC_ECAP_H_ */
