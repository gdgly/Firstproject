/*=======================================================================
 *Subsystem:   裸机
 *File:        Light.h
 *Author:      WenYuhao
 *Description: 
 ========================================================================
 * History:    
 * 1. Date:
      Author:
      Modification:
========================================================================*/

#ifndef _PORTCONTROL_H_
#define _PORTCONTROL_H_  

  #include  "TypeDefinition.h"
  #include  "MC9S12XEP100.h"
  #include  "derivative.h"
  #include  "GPIO.h"

    //1.灯状态宏定义(根据实际电路定义) 
    #define Light_ON        0    //灯亮(对应低电平)
    #define Light_OFF       1    //灯暗(对应高电平)
    
    //2.灯控制引脚宏定义
    #define LED1_PORT   PT   //
    #define LED1_pin    0    //
    
    #define LED2_PORT   PT   //
    #define LED2_pin    1    // 
    
    #define LED3_PORT   PB   //
    #define LED3_pin    4    //   
      

    
    //#define Relay_ChgHeat_PORT    PB  //加热时有单独进行PORTB_PB3设置
    //#define Relay_ChgHeat_pin      3
    
    //#define Relay_VoltDetect_PORT PB
    //#define Relay_VoltDetect_pin   6
    
    
//构件函数声明区
uint8 Light_Init(uint16 port,uint8 name,uint8 state);   
void Light_Control(uint16 port,uint8 name,uint8 state);
void Light_Change(uint16 port,uint8 name);
uint8 Light_StateGet(uint16 port,uint8 pin);
//****************************继电器***************************************//
uint8 Relay_Init(uint16 port,uint8 name,uint8 state);
void Relay_Control(uint16 port,uint8 name,uint8 state);
uint8 Relay_StateGet(uint16 port,uint8 pin);
   
//===========================================================================

#endif 