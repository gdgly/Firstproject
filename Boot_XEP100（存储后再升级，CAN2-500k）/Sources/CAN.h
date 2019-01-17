#ifndef _CAN_H
#define _CAN_H


#define ID_BMU 	             0x0001  //ID����Ϊ01,�Ӱ�Ϊ02�Դ�����
#define ID_CSSU1 	           0x0002
#define ID_CSSU2 	           0x0003
#define ID_CSSU3 	           0x0004

#define ID_BootState         0x01FF  //BOOT����״̬����ֵ

#define RxBufSize 80

 
typedef struct                    //���ͱ��ĵĽṹ��
{
    unsigned long id;             //ID��
    Bool RTR;                     //�Ƿ�ΪԶ����
    unsigned char data[8];        //�����������
    unsigned char len;            //can���͵����ݳ���Ϊ8  
    unsigned char prty;           //CANͨ�Ŵ������ȼ��趨
} CAN_MSG;

extern CAN_MSG can_send;

extern UINT8 RxIn;               //next available location in the Rx queue
extern UINT8 RxOut;              //next character to be removed from the Rx queue

extern UINT8 RxBuff[RxBufSize];  //receive queue
extern volatile UINT8 RxBAvail;  //number of bytes left in the Rx queue

#pragma CODE_SEG RAM_CODE
extern  interrupt void CAN3RxISR(void); 
#pragma CODE_SEG DEFAULT

extern INT8 getchar(void);
extern void INIT_CAN2(void); 
extern Bool MSCAN2GetMsg(CAN_MSG *msg);
extern Bool MSCAN2SendMsg(CAN_MSG msg);
extern void CAN_SendByte(UINT8 data);
#endif