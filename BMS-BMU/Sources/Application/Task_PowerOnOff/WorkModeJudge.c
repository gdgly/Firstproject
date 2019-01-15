/*=======================================================================
 *Subsystem:   裸机
 *File:        WorkModeJudge.C
 *Author:      WenYuhao
 *Description: 通信：
               接口：
               波特率：
               只有自检成功后才能闭合响应的开关
/* ========================================================================
 * History:    修改历史记录列表；
 * 1. Date:    
      Author:  
      Modification: 
========================================================================*/
#include  "includes.h"  

WorkStateJudge_T  g_WorkStateJudge;

/*=======================================================================
 *函数名:      ADC_CC2_State
 *功能:        电池包霍尔传感器电流的检测
 *参数:        无         
                      
 *返回：       Current:霍尔传感器通道的转换结果
 *说明：       ADC 时钟频率：2MHz
========================================================================*/
static
uint16 ADC_CC2_State(void) 
{
  uint16 cc2_state;
  cc2_state = ADC_Value(QCharge_CC2); 
  return cc2_state;  
}

/*=======================================================================
 *函数名:      WorkMode_DelayTime(uint16)  
 *功能:        工作状态判断延时
 *参数:        ts:延时节拍数       
 *返回：       无
 *说明：       进行AD采集时进行短暂的延时
========================================================================*/
static
void WorkMode_DelayTime(uint16 ts)
{
  uint16 i;
  for(i=0; i<ts; i++);
} 
/*=======================================================================
 *函数名:      WokeModeJudgment(void)  
 *功能:        判断充电还是放电状态
 *参数:        无       
 *返回：       无
 *说明：       任务时间到标志位给1，10ms执行一次
========================================================================*/
uint8 WokeModeJudgment(void)                                
{  
  //判断车的状态(充电or放电)
  uint8 state;
  state = Fault_Charge_OffLine(); //以充电报文来判断充电状态
  //state = !state;   
  return state;   
  /*uint8  i=0, q=0;
  for(i=0; i<10; i++)
  {
    if(ADC_CC2_State() <= 3000)
    {
      q++;
      WorkMode_DelayTime(20);
    }
  }   
  if(q == 10)//连续10次是相同的状态
  {
    return MODE_CHARGE;   // ChagMode == 1
  }
  else
  {
    return MODE_DISCHARGE;
  }*/
}