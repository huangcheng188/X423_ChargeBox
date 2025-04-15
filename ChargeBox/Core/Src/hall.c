#include "hall.h"
#include "main.h"

hall_t hall;

hall_m get_hall_mode(hall_t *phall)
{
  return phall->hall_mode;
}

void hall_set_mode(hall_t *phall, hall_m mode)
{
    phall->hall_mode = mode;
}

void hall_process(hall_t *phall, void(*cb)(hall_m mode))
{
    if(phall == NULL)
    return ;

    static hall_m mode = HALL_UNKNOW;
    static uint8_t first_start = 0;    //first start ,can't get typec status

    if(!first_start){
      if(HAL_GPIO_ReadPin(HALL_OUT_GPIO_Port, HALL_OUT_Pin)){
          //phall->hall_mode = HALL_FAR;
          hall_set_mode(&hall, HALL_FAR);
      }else{
          //phall->hall_mode = HALL_NEAR;
          hall_set_mode(&hall, HALL_NEAR);
      }
      first_start = 1;
    }
    


    if(mode == phall->hall_mode){
      return ;
    }

    switch(phall->hall_mode){ 
      case HALL_FAR:
          printf("HALL_FAR\r\n");
          break;
      case HALL_NEAR:
          printf("HALL_NEAR\r\n");
          break;
      default:
          break;

    }
    if(cb)
      cb(phall->hall_mode);

    mode = phall->hall_mode;

}