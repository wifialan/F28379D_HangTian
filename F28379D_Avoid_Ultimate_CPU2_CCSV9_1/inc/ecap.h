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

typedef struct ecap_dev{

    float distance;

    int ecap1_cap_counter;
    int ecap2_cap_counter;
    int ecap3_cap_counter;


    volatile uint32_t ecap1_time_ns;
    volatile uint32_t ecap1_time_us;

    volatile uint32_t ecap2_time_ns;
    volatile uint32_t ecap2_time_us;

    volatile uint32_t ecap3_time_ns;
    volatile uint32_t ecap3_time_us;

    volatile float ecap1_distance_float;
    volatile float ecap2_distance_float;
    volatile float ecap3_distance_float;


    char  ecap1_distance[10];
    char  ecap2_distance[10];
    char  ecap3_distance[10];

}ECAP_DEV;

void init_ecap();
void InitEcapture1();
void InitEcapture2();
void InitEcapture3();

void ecap1_start();
void ecap2_start();
void ecap3_start();
void ecap1_stop();
void ecap2_stop();
void ecap3_stop();

__interrupt void ecap1_isr_signal(void);
__interrupt void ecap2_isr_signal(void);
__interrupt void ecap3_isr_signal(void);



#endif /* INC_ECAP_H_ */
