/*=======================================================================
 *Subsystem:   ���
 *File:        main.C
 *Author:      Wenm
 *Description: ͨ�ţ�
               �ӿڣ�
               �����ʣ�
 ========================================================================
 * History:    �޸���ʷ��¼�б�
 * 1. Date:    
      Author:  
      Modification:          
========================================================================*/

#include "PIT.h"

/*=======================================================================
 *������:      ����������
 *����:        1.ϵͳ��ʼ����
               2.�������Ĵ�����
               3.�������ѯ
 *����:        �� 
 *���أ�       ��
 *˵����       ����������ʼ���������ִ��
========================================================================*/  
void main(void) 
{         
  //System_init(); //ϵͳ�ײ㺯����ʼ��  
  //�˴���ѯ�����Լ�,�Լ�ɹ�����������(0:�Լ�ɹ�,1:�Լ�ʧ��,��Ҫ�����Լ�)
  while(BMS_CheckSelf(&(PIT_Time_10ms.Period_10ms), WokeModeJudgment())!=0); 
  for(;;) 
  {
     Task_Handle();//������ѯ
  } 
}