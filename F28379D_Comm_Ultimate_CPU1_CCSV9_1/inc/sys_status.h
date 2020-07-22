/*
 * sys_status.h
 *
 *  Created on: 2019Äê12ÔÂ31ÈÕ
 *      Author: multimicro
 */

#ifndef INC_SYS_STATUS_H_
#define INC_SYS_STATUS_H_

#include "stdint.h"

#define QFSK_DATA_SEND_SUCCEEDED 100

struct sys_status{

#define STATUS_SEND 0
#define STATUS_RECE 1
#define STATUS_DEMOD_BEGIN 2
#define STATUS_DEMOD_FINISH 3
#define STATUS_DEMOD_RUNNING 4
#define STATUE_DEMOD_STOP 5

    char send_or_rece_state;
    char demod_state;
};


#endif /* INC_SYS_STATUS_H_ */
