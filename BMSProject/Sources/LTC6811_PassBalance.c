/*=======================================================================
 *Subsystem:   裸机
 *File:        LTC6811_PassBalance.C
 *Author:      WenM
 *Description: 通信：
               接口：
               波特率：
 ========================================================================
 * History:    修改历史记录列表
 * 1. Date:   
      Author:  
      Modification:  
      
========================================================================*/
#include  "LTC6811_PassBalance.h"               

uint8 Balance_CFGR[NUM_IC][6];
/*=======================================================================
 *函数名:      Balance_Config();
 *功能:        对LTC6804的配置
 *参数:        无       
 *返回：       无
 *说明：       IC=3;GPIO=0XFF; REFON=1; SWTEN=0;  ADCOPT=0;  VUV=3.2; VOV=4.2; DCTO=0;
========================================================================*/
void LTC6811_BalanceCMDSend( uint8 *CFG,uint8 gpio, uint8 refon,uint8 swtrd, uint8 adcopt,
                 float  vuv,float   vov,uint16 ddc,uint8 dcto) 
{           
  uint8 char1,char2,char3;
  uint8 current_ic;                             /* 对LTC6804_1的初始化命令配置参数 */

  *CFG++=(gpio<<3)+(refon<<2)+(swtrd<<1)+adcopt;
  
  *CFG++=(uint8)(((uint16)vuv*10000/16-1)&0x00ff);
  
  char1=(uint8)((((uint16)vov*10000/16)&0x000f)<<4);

  char2=(uint8)((((uint16)vuv*10000/16-1)&0x0f00)>>8);
   
  *CFG++=char1+char2;
  
  *CFG++= (uint8)(((uint16)(vov*10000/16)&0x0ff0)>>4);
  
  *CFG++= ddc;
  
  char3 = (uint8)(ddc>>8);
  
  char3 = char3 &0x0f; 
 
  *CFG++=(dcto<<4)+ char3; 
}
/*=======================================================================
 *函数名:      Passive_Balance（void）
 *功能:        对电池组进行被动均衡
 *参数:        state1：第1个芯片的均衡状态范围：1~12
               state2：第2个芯片的均衡状态范围：1~12
               state3：第3个芯片的均衡状态范围：1~12
               time  : 均衡时间长度
 *返回：       无
 *说明：       被动均衡函数
========================================================================*/
uint8 LTC6811_BalanceControl(uint16 state1, uint16 state2, uint16 state3, uint8 time) 
{
  uint16 state[3]={0,0,0};
  if((state1<1||state1>NUM_Battery)||(state2<1||state2>NUM_Battery)||((state3<1||state3>NUM_Battery)))
  {
    return 1;
  }
  state[1] = 1<<(state1-1);
  if(state[1]>NUM1_Batper_front)
  {
    state[1] = 1<<(state1-1+6-NUM1_Batper_front);
  }
  state[2] = 1<<(state2-1);
  if(state[1]>NUM1_Batper_front)
  {
    state[2] = 1<<(state2-1+6-NUM1_Batper_front); 
  }
  state[3] = 1<<(state3-1);
  if(state[1]>NUM1_Batper_front)
  {
    state[3] = 1<<(state3-1+6-NUM1_Batper_front); 
  }
  
  LTC6811_BalanceCMDSend(&Balance_CFGR[0][0], DGPIO, DREFON, DSWTRD, DADCOPT, UNDER_V, OVER_V, state[1], time) ;   /* 配置值赋给结构体,更改最后两个变量值；*/                                           

  LTC6811_BalanceCMDSend(&Balance_CFGR[1][0], DGPIO, DREFON, DSWTRD, DADCOPT, UNDER_V, OVER_V, state[2], time) ;   /* 配置值赋给结构体,更改最后两个变量值；*/  

  LTC6811_BalanceCMDSend(&Balance_CFGR[2][0], DGPIO, DREFON, DSWTRD, DADCOPT, UNDER_V, OVER_V, state[3], time) ;   /* 配置值赋给结构体,更改最后两个变量值；*/  

  LTC6804_wrcfg(NUM_IC, Balance_CFGR);
  
  return 0; 
}



