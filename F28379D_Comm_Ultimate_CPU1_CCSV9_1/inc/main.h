/*
 * main.h
 *
 *  Created on: 2018��11��29��
 *      Author: multimicro
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

void f28379d_system_init(void);
void ipc_send_data(uint32_t data);

__interrupt void cpu_timer0_isr(void);
interrupt void ipc0_isr(void);

#endif /* INC_MAIN_H_ */
