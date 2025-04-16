#include <stdio.h>
#include "stm32g0xx_hal.h"

extern I2C_HandleTypeDef hi2c2;

#define GPIO1_master HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)     //for test

#define my_delay_ms(x) HAL_Delay(x)

//******************************************************
unsigned char Master_Default_Reg_Value[0x30]={0x88,0xe9,0x00,0x00,0x10,0x02,0x00,0x80,
                                              0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x42,
                                              0x6c,0x6c,0x10,0x11,0x00,0x3f,0x00,0x5c,
                                              0x14,0x46,0x00,0x00,0xff,0x03,0x02,0x6e,
                                              0x00,0x00,0x0c,0x0c,0x00,0xff,0xff,0xff,
                                              0xff,0xff,0x09,0x00,0x00,0x00,0x11,0x00};     


      

static int i2c_write (uint8_t i2c_addr, uint8_t register_addr, char* buffer, uint8_t Nbyte );
static int i2c_read (uint8_t i2c_addr, uint8_t register_addr, char* buffer, uint8_t Nbyte );

void Master_Glass_Plug_In_Detection_L(void);

void Master_Glass_Plug_Out_Detection_L(void);

void Master_Intb_To_VoutL_Uart(void);
void Master_Intb_To_VoutR_Uart(void);
void Master_Exit_Uart(void);
void Master_Enter_Charge_Mode(void);

void print_help_information(void);
