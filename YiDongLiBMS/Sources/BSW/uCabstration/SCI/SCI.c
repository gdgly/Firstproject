/*=======================================================================
 *Subsystem:   ���
 *File:        SCI.C
 *Author:      WenMing
 *Description: ͨ�ţ�SCI2.
               �ӿڣ�PJ0��RXD����PJ1��TXD��
               �����ʣ�
 ========================================================================
 * History:        // �޸���ʷ��¼�б���ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸��߼��޸����ݼ���
 *1. Date:         2018 - 5 - 29
     Author:       ZWB
     Modification: GPRSrxdata������������޸ģ������˱궨���ܵĽ��ܡ�
========================================================================*/

#include  "SCI.h"    //ͷ�ļ�����      

/*=======================================================================
 *������:      SCI_Init
 *����:        ��ʼ��SCI
 *����:        SCINo:SCI2
               sysclk��ʱ������ʱ��:32MHz
               baud��SCIͨ�Ų�����       
 *���أ�       ��
 *˵����       
========================================================================*/
uint8 SCI1_Init(void) 
{    
  uint16 SCI_temp;
  
  SCI_temp = 32000000/16/9600;
  SCI1BD = SCI_temp;              // ����SCI0������Ϊ12500��BAUD����BUS_CLOCK=32MHz
                                                 
  SCI1CR1 = 0x00;                 // ����SCI0Ϊ����ģʽ����λ����λ������żУ��( �Ƿ���Ҫ��״̬��ȷ�ϣ�)
  SCI1CR2 = 0x08;                 // �����������ݣ���ֹ�жϹ���
  
  return(Init_Normal_SCI);   
}

/*=======================================================================
 *������:      SCI2_Send_Byte
 *����:        SACI����һ���ֽ�
 *����:        data:Ҫ���͵��ֽ�       
 *���أ�       ��
 *˵����       
========================================================================*/
static
uint8 SCI1_Send_Byte(uint8 data) 
{ 
    uint8 cnt[1];
    do
    {
      if(++cnt[0] == 200)
      {
        return (SendMsg_Fault_SCI_SCI1SR1_TDRE);
      }
    }
    while(!SCI1SR1_TDRE);             // �ȴ��������ݼĴ�������������Ϊ��    
    SCI1DRL = data;  
    return(SendMsg_SCI);   
}

/*=======================================================================
 *������:      SCI2_Send_NByte
 *����:        SACI���Ͷ���ֽ�
 *����:        string:Ҫ���͵��ֽ�ָ��       
 *���أ�       ��
 *˵����       
========================================================================*/
uint8 SCI1_Send_NByte(uint8 n, uint8 string[]) 
{ 
  uint8 i;
  uint8 SCI_state; 
  for(i = 0; i < n; i++)         //����ʱ������
  { 
    
    SCI_state = SCI1_Send_Byte(string[i]);
  } 
  return(SCI_state); 
}