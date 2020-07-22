/*
 * fsk.h
 *
 *  Created on: 2018Äê12ÔÂ1ÈÕ
 *      Author: multimicro
 */

#ifndef INC_FSK_H_
#define INC_FSK_H_

void qfsk_send_data(Uint32 data);
void qfsk_freq_select(Uint32 freq);
Uint16 qfsk_data_send_len(Uint32 data);
void lfm(Uint16 start_freq, Uint16 end_freq, Uint16 freq_step);

#endif /* INC_FSK_H_ */
