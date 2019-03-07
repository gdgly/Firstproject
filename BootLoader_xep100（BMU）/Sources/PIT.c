/*=======================================================================
 *Subsystem:   裸机
 *File:        PIT.C
 *Author:      WENMING
 *Description: 通信：
               接口：
               波特率：
 ========================================================================
 * History:    修改历史记录列表
 * 1. Date:
      Author:
      Modification:
========================================================================*/
//#include "Includes.h"                  
 //构件函数实现

//PITInit:定时器初始化函数----------------------------------------------*
//功  能:定时器初始化，中断一次时间为1/38秒                             *
//参  数:无                                                             *
//返  回:无                                                             *
//----------------------------------------------------------------------*

#include <string.h>
#include "Types.h"
#include "MC9S12XEP100.h"
#include "PIT.h"  
  
void init_PIT0(void)
{
   PITMTLD0 = 249;     //为0通道8位计数器赋值
   PITLD0 = 1279;      //为0通道16位计数器赋值   //(249+1)*(1279+1)/32M=10ms,本程序为10ms执行周期
   PITMUX_PMUX0 = 0;   //第0通道使用微计数器0
   PITCE_PCE0 = 1;     //第0通道计数器工作 
   PITCFLMT = 0x80;    //使能PIT模块
   PITINTE_PINTE0 = 1; //0通道定时器定时中断被使能
}

//若出现CAN返回0xC4后不出现0x02的正常返回值，那么将PIT中断的时间改为15~20ms
//即:PITLD0 = 2559,其他寄存器不变


