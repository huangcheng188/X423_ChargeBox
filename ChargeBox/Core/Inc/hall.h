#ifndef __HALL_H
#define __HALL_H

#include "stm32g0xx_hal.h"



typedef enum hall_enum{
    HALL_NEAR = 0,
    HALL_FAR,
    HALL_UNKNOW
}hall_m;

typedef struct hall_struct{
    hall_m hall_mode;
}hall_t;

extern hall_t hall;

hall_m get_hall_mode(hall_t *phall);
void hall_set_mode(hall_t *phall, hall_m mode);
void hall_process(hall_t *phall, void(*cb)(hall_m mode));

#endif //__HALL_H
