
#include "YHM4101.h"
#include "YHM4001REG.h"

//******************YHM4001 register define*************************************
#define YHM4001_MASTER 0xC8   //8bit address


MAX32630FTHR pegasus(MAX32630FTHR::VIO_1V8);  //VIO_3V3

USBSerial pc;  // Virtual serial port over USB
DigitalInOut GPIO1_master(P5_6);         // for master Interrupt


I2C i2c(P3_4,P3_5);  //SDA  SCL 


//***************************I2c Write function *****************************
 static int i2c_write (uint8_t i2c_addr, uint8_t register_addr, char* buffer, uint8_t Nbyte )
{
    int ret;
    char *tmp_buffer;
        
    tmp_buffer = (char*)malloc(sizeof(char)*(Nbyte+1));
        
    /* First, send device address. Then, send data and STOP condition */
    tmp_buffer[0] = register_addr;
    memcpy(tmp_buffer+1, buffer, Nbyte);

    ret = i2c.write(i2c_addr, tmp_buffer, Nbyte+1, false);
    free(tmp_buffer);
    return ret;
}
//***************************I2c Read function *****************************
static int i2c_read (uint8_t i2c_addr, uint8_t register_addr, char* buffer, uint8_t Nbyte )
{
    int ret;
    
    ret = i2c.write(i2c_addr, (const char*)&register_addr, 1, true);
    if(!ret) {
        ret = i2c.read((i2c_addr|0x01), buffer, Nbyte, false);
    }
 
    return ret; 
}



void print_help_information(void)
{
  pc.printf("L 64        //-----list all of register value\r\n"); 
  pc.printf("S 64  0     //-----read reg0 value\r\n"); 
  pc.printf("W 64  0  cd //-----write reg 0==0xcd\r\n"); 

  pc.printf("D 91       //Plug IN detection_Left channel\r\n"); 
  pc.printf("D 92       //Plug OUT detection_Left channel\r\n"); 
  pc.printf("D 93       //Master_Intb_To_VoutL_Uart\r\n"); 
  pc.printf("D 94       //Master_Intb_To_VoutR_Uart\r\n"); 
  pc.printf("D 95       //Master_Exit_Uart\r\n");    
  pc.printf("D 96       //Master_Enter_Charge_Mode\r\n");            
}
//*******************************************************************************************************************
void print_help_information(void)
{
  pc.printf("L 64        //-----list all of register value\r\n"); 
  pc.printf("S 64  0     //-----read reg0 value\r\n"); 
  pc.printf("W 64  0  cd //-----write reg 0==0xcd\r\n"); 

  pc.printf("D 91       //Plug IN detection_Left channel\r\n"); 
  pc.printf("D 92       //Plug OUT detection_Left channel\r\n"); 
  pc.printf("D 93       //Master_Intb_To_VoutL_Uart\r\n"); 
  pc.printf("D 94       //Master_Intb_To_VoutR_Uart\r\n"); 
  pc.printf("D 95       //Master_Exit_Uart\r\n");    
  pc.printf("D 96       //Master_Enter_Charge_Mode\r\n");            
}
//*******************************************************************************************************************
void Master_Glass_Plug_In_Detection_L(void)
{
  char temp_txdata[20];
  char temp_rxdata[20];  
  int i;
  char L_status=0;  //0== in box, 1==out of box

  for (i=0x0e;i<0x20;i++)
    {
        if(i!=0x13 )
            {
                temp_txdata[0]=Master_Default_Reg_Value[i];
                i2c_write(YHM4001_MASTER, i, &temp_txdata[0], 1);
            }
    }      

    i2c_read(YHM4001_MASTER, 0x6, &temp_rxdata[0], 4);  //master clear interrupt


    temp_txdata[0]=0xbf;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x0a, &temp_txdata[0], 1);   //enable L_DET INT


    temp_txdata[0]=0xff;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x0b, &temp_txdata[0], 1);   ///mask all of interrupt 
    i2c_write(YHM4001_MASTER, 0x0c, &temp_txdata[0], 1);   ///mask all of interrupt                                                                                                    
    i2c_write(YHM4001_MASTER, 0x0d, &temp_txdata[0], 1);   ///mask all of interrupt
                                                                                      
    temp_txdata[0]=0x11;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x13, &temp_txdata[0], 1);   // master switch OFF

    temp_txdata[0]=0x08;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x14, &temp_txdata[0], 1);   //enable VOUTL  2uA current source
    
    my_delay_ms(5);
    
    i2c_read(YHM4001_MASTER, 0x6, &temp_rxdata[0], 4);  //master clear interrupt

    i2c_read(YHM4001_MASTER, 0x2, &temp_rxdata[2], 1);  //read 02 register

    if((temp_rxdata[2]&0x40)==0x40)
     {
         L_status=1;  // 1==out of box
         pc.printf("Master: L is  plug out status, now do the plug in test \r\n");   
     }
    else
    {
        temp_txdata[0]=0x91;                                                           
        i2c_write(YHM4001_MASTER, 0x13, &temp_txdata[0], 1);   // master switch L ON

         L_status=0;  //0== in box
         pc.printf("********Master: L is  plug in, so exit the plug in test program*******\r\n");   


    } 
//-------------now, wait the interrupt pin is low----------------

if(L_status==1)
{
     do{
           my_delay_ms(500);
           pc.printf(".");        
        }while(GPIO1_master==1);  //wait the  intb==0

    i2c_read(YHM4001_MASTER, 0x6, &temp_rxdata[6], 4);  //read 06~09 register
    i2c_read(YHM4001_MASTER, 0x2, &temp_rxdata[2], 1);  //read 02 register
    
     pc.printf("\r\nreg [6]==%02x \r\n",temp_rxdata[6]); 
     pc.printf("reg [7]==%02x \r\n",temp_rxdata[7]); 
     pc.printf("reg [8]==%02x \r\n",temp_rxdata[8]); 
     pc.printf("reg [9]==%02x \r\n",temp_rxdata[9]); 

     pc.printf("reg [2]==%02x \r\n",temp_rxdata[2]); 

    if(((temp_rxdata[6]&0x40)==0x40) && ((temp_rxdata[2]&0x40)==0x00))
    {
       pc.printf("\r\n");   
       pc.printf("Master: L plug in\r\n");   
       pc.printf("***************Master: L plug in detect finished****************\r\n");  
       
       temp_txdata[0]=0x91;                                                                                                                 
       i2c_write(YHM4001_MASTER, 0x13, &temp_txdata[0], 1);   // master L switch path ON

    }

}

}

//*******************************************************************************************************************
void Master_Glass_Plug_Out_Detection_L(void)
{
  char temp_txdata[20];
  char temp_rxdata[20];  
  int i;
  char L_status;

  L_status=0;  //0== in box, 1==out of box

 

do{
  for (i=0x0e;i<0x20;i++)
    {
        if(i!=0x13 )
            {
                temp_txdata[0]=Master_Default_Reg_Value[i];
                i2c_write(YHM4001_MASTER, i, &temp_txdata[0], 1);
            }
    }      

      i2c_read(YHM4001_MASTER, 0x6, &temp_rxdata[0], 4);  //master clear interrupt

    temp_txdata[0]=0xff;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x0a, &temp_txdata[0], 1);   //mask all of interrupt 
                                                                                                                
    i2c_write(YHM4001_MASTER, 0x0b, &temp_txdata[0], 1);   ///mask all of interrupt 
    i2c_write(YHM4001_MASTER, 0x0c, &temp_txdata[0], 1);   ///mask all of interrupt   


    temp_txdata[0]=0xf7;                                                                                                     
    i2c_write(YHM4001_MASTER, 0x0d, &temp_txdata[0], 1);   //enable L_MIN INT 

    temp_txdata[0]=0x00;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x14, &temp_txdata[0], 1);   //disable 2uA current source

                                                                                      
    temp_txdata[0]=0x91;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x13, &temp_txdata[0], 1);   // master switch L ON

    temp_txdata[0]=0xD0;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x12, &temp_txdata[0], 1);   //set Imin=0.8mA

    temp_txdata[0]=0x80;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x16, &temp_txdata[0], 1);   //enable Icmp

    temp_txdata[0]=0xD1;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x13, &temp_txdata[0], 1);   //enable L switch current sense

    temp_txdata[0]=0x7c;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x10, &temp_txdata[0], 1);   //enable L current sense auto scale 

   
    my_delay_ms(5);
    
    i2c_read(YHM4001_MASTER, 0x6, &temp_rxdata[0], 4);  //master clear interrupt

//-------------now, wait the interrupt pin is low----------------

     do{
           my_delay_ms(500);
           pc.printf(".");        
        }while(GPIO1_master==1);  //wait the  intb==0

    i2c_read(YHM4001_MASTER, 0x6, &temp_rxdata[6], 4);  //read 06~09 register
    i2c_read(YHM4001_MASTER, 0x5, &temp_rxdata[5], 1);  //read 05 register

     pc.printf("\r\nreg [6]==%02x \r\n",temp_rxdata[6]); 
     pc.printf("reg [7]==%02x \r\n",temp_rxdata[7]); 
     pc.printf("reg [8]==%02x \r\n",temp_rxdata[8]); 
     pc.printf("reg [9]==%02x \r\n",temp_rxdata[9]); 

     pc.printf("reg [5]==%02x \r\n",temp_rxdata[5]); 


    if(((temp_rxdata[9]&0x08)==0x08) && ((temp_rxdata[5]&0x08)==0x08))   //L switch min current
    {
       pc.printf("\r\n");   
       pc.printf("Master: L  Current Min\r\n");   
       pc.printf("Now do the 2uA Isource Detect\r\n");

    temp_txdata[0]=0xff;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x0a, &temp_txdata[0], 1);   //mask all of interrupt 
                                                                                                                
    i2c_write(YHM4001_MASTER, 0x0b, &temp_txdata[0], 1);   ///mask all of interrupt 
    i2c_write(YHM4001_MASTER, 0x0c, &temp_txdata[0], 1);   ///mask all of interrupt         
    i2c_write(YHM4001_MASTER, 0x0d, &temp_txdata[0], 1);   ///mask all of interrupt  


       temp_txdata[0]=0x11;  
       i2c_write(YHM4001_MASTER, 0x13, &temp_txdata[0], 1);  //master switch off

       temp_txdata[0]=0x08;  
        i2c_write(YHM4001_MASTER, 0x14, &temp_txdata[0], 1);  //enable VOUTL  2uA current source

       my_delay_ms(50);
       
       i2c_read(YHM4001_MASTER, 0x2, &temp_rxdata[2], 1);  //read 02 register

       if((temp_rxdata[2]&0x40)==0x40)   //VOUTL > vth2x
        {
         pc.printf("Master: L plug out\r\n"); 

         temp_txdata[0]=0x00;  
         i2c_write(YHM4001_MASTER, 0x14, &temp_txdata[0], 1);  //No   2uA current source

         L_status=1;   //out of box

        }
       else
       {
         pc.printf("Master: L No Plug out, is a current dither\r\n"); 
         pc.printf("Master: need Re-do the plug out detect\r\n"); 

         temp_txdata[0]=0x00;  
         i2c_write(YHM4001_MASTER, 0x14, &temp_txdata[0], 1);  //No   2uA current source

         L_status=0;   //in of box           
       } 

    }

}while(L_status==0);

  pc.printf("****************Master: Plug Out Detect finished************************\r\n"); 

}

//*******************************************************************************************************************
void Master_Intb_To_VoutL_Uart(void)
{
  char temp_txdata[2];

    temp_txdata[0]=0x11;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x13, &temp_txdata[0], 1);   //VIN to VOUTL&VOUTR OFF 

    temp_txdata[0]=0x52;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x0f, &temp_txdata[0], 1);   //INTB to VOUTL ON，level shift path

}
//*******************************************************************************************************************
void Master_Intb_To_VoutR_Uart(void)
{
  char temp_txdata[2];

    temp_txdata[0]=0x11;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x13, &temp_txdata[0], 1);   //VIN to VOUTL&VOUTR OFF 

    temp_txdata[0]=0x4A;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x0f, &temp_txdata[0], 1);   //INTB to VOUTR ON，level shift path
}
//*******************************************************************************************************************
void Master_Exit_Uart(void)
{
  char temp_txdata[2];

    temp_txdata[0]=0x42;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x0f, &temp_txdata[0], 1);   //INTB to VOUTL OFF， INTB to VOUTR OFF
}
//*******************************************************************************************************************
void Master_Enter_Charge_Mode(void)
{
  char temp_txdata[2];

    temp_txdata[0]=0x42;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x0f, &temp_txdata[0], 1);   //INTB switch OFF 

    temp_txdata[0]=0x99;                                                                                                                   
    i2c_write(YHM4001_MASTER, 0x13, &temp_txdata[0], 1);   //VOUTL & VOUTR path ON
}
