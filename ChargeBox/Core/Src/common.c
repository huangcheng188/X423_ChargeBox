#include "main.h"
#include "common.h"
#include <stdio.h>


#define PROJECT_VERSION "V1.0.0"

void project_version_printf(void)
{
  printf("*****************************************\r\n");
  printf("Firmware Version: %s\r\n", PROJECT_VERSION);
  printf("Compile Date: %s Time: %s\r\n", __DATE__, __TIME__);
  printf("*****************************************\r\n");
}


timer_t timer = {0};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    //printf("timer--\r\n");
    if(timer.time_1000ms){
        timer.time_1000ms--;
    }
}

void heartrate_callback(void)
{
    static int cnt = 0;
    printf("MCU:%d\r\n", cnt++);
}

//tick_cnt*10ms = period time
void stm32_heartrate(uint32_t tick_cnt, void (*callback)(void))
{
    if(timer.time_1000ms == 0){
        timer.time_1000ms = tick_cnt;
        callback();
    }
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == HALL_OUT_Pin){
    printf("HALL_OUT_Pin Rising\r\n");
  }else{
    printf("Unkown Rising\r\n");
  }
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == HALL_OUT_Pin){
    printf("HALL_OUT_Pin Falling\r\n");
  }else{
    printf("Unkown Falling\r\n");
  }
}

