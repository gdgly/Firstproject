/*=======================================================================
 *Subsystem:   ���
 *File:        Task_BalanceControl.C
 *Author:      WenM
 *Description: ͨ�ţ�
               �ӿڣ�
               �����ʣ�
 ========================================================================
 * History:    �޸���ʷ��¼�б�
 * 1. Date:   
      Author:  
      Modification:  
      
========================================================================*/

#include  "Includes.h"

BalanceInfo_T balance_receive;
/*=======================================================================
 *������:      BalanceControl_Strategy(flaot, uint8, uint16, uint32, uint8, float)
 *����:        �Ե������б�������
               ������Ʋ���:����������ƽ����ѹbalancevoltʱ���о���
 *����:        curr���жϵ����Ĵ�С,ֻ������С��3A(��̬)���о���
               faultflg:  �ڲ����ֹ��ϵ�����²Ž��о���
               voltmax��  ����ѹ
               totalvolt��25����ص���ѹ,������ƽ����ѹ
               balacenod: ����Ľڵ�                 
 
 *���أ�       uint8:     0��ʾ��������
                          1��ʾ������ִ��������ֹͣ����
                          2��ʾδ�ﵽ�������������о���
 *˵����       �������⺯��
========================================================================*/
static
uint8 BalanceControl_Strategy(float curr, uint8 faultflg, uint16 voltmax, uint32 totalvolt, uint8 balacenod, uint16 balancevolt)
{
  uint8 tskstate=2;                   //����2��ʾδ���о���
  static uint16 cnt;
  uint8 balanceNum;
  
  if(curr>=10 && (faultflg == 0) && (1 == balance_receive.model_work))//MODE_CHARGE)) //ֻ���ڳ��Ĺ����е�������5A�ſ���
  {
    if((voltmax - (totalvolt/25.0)) > balancevolt)
    { 
       if(++cnt*BALANCEPERIO/1000>=3)//����2s,�����������Ƿ�����?
       {
          
         PTT_PTT0 = 0;//������ʾ�ƿ���
         cnt = 3000/BALANCEPERIO;
         if(balacenod <= NUM1_Batper_true)
         {
           tskstate = LTC6811_BalanceControl(balacenod, 0x00, 0x00, 1); 
         }
         else if(balacenod <= (NUM1_Batper_true+NUM2_Batper_true))
         {
            balanceNum = balacenod-NUM1_Batper_true;
           tskstate =  LTC6811_BalanceControl(0x00, balanceNum, 0x00, 1);
         }
         else if(balacenod <= (NUM1_Batper_true+NUM2_Batper_true+NUM3_Batper_true))
         {
            balanceNum = balacenod-NUM1_Batper_true-NUM2_Batper_true;
            tskstate = LTC6811_BalanceControl(0x00, 0x00, balanceNum, 1);
         }
         else
         {
            return 1;
         } 
       }
     } 
     else
     {
        cnt = 0;
     }
  }
  else
  {
     LTC6811_BalanceControl(0x00, 0x00, 0x00, 0); //δ�������������о��� 
     return 2;
  }
  return tskstate;
}

/*=======================================================================
 *������:      Task_BalanceControl_ON(void)
 *����:        ���⿪����������
 *����:        ��       
 *���أ�       ��
 *˵����       ���ݾ�����Ʋ��Կ����������,ֻ�ڵ�ѹ�ɼ����֮��������
========================================================================*/
void Task_BalanceControl_ON(void)
{
   uint8 balancestate;
          
  if(balance_receive.total_volt < 5000)
  {
      balance_receive.flag = 1;
  } 
  else
  {    
      balance_receive.flag = 0;
  }
          
   BalanceControl_Strategy(balance_receive.current, balance_receive.flag,\
                           VoltInfo.CellVolt_Max, balance_receive.total_volt,(VoltInfo.CellVolt_MaxNode+1), 500); 
   
   memset(&balance_receive,0,sizeof(balance_receive));
   
   Task_Flag_Counter.Counter_Balance_open++;                                       
}
/*=======================================================================
 *������:      Task_BalanceControl_OFF(void)  
 *����:        ����رտ�������
 *����:        ��       
 *���أ�       ��
 *˵����       �ڵ�ѹ�ɼ�֮ǰ�رվ��⿪��
========================================================================*/
void Task_BalanceControl_OFF(void)
{
   LTC6811_BalanceControl(0, 0, 0, 0);//�رվ��⹦��
   PTT_PTT0 = 1;
   Task_Flag_Counter.Counter_Balance_close++;
}