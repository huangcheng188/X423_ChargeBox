#ifndef __COMMON_H
#define __COMMON_H

#include "stm32g0xx_hal.h"

typedef struct timer_struct{
    uint8_t time_1000ms;        //1000ms flag
}timer_t;
extern timer_t timer;

void heartrate_callback(void);
void stm32_heartrate(uint32_t tick_cnt, void (*callback)(void));

#endif //__COMMON_H
