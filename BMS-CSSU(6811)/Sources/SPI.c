#include "Includes.h"

/*=======================================================================
 *������:      SPI_Init(uint8) 
 *����:        ����ʱ�亯��
 *����:        ��       
 *���أ�       ��
 *˵����       ����ʱ�䵽��־λ��1
========================================================================*/

/*************************************************************/
/*                       SPI��ʼ������                         */
/*************************************************************/
void SPI1_Init(void) 
{
    SS2_Dir = 1;
    SPI1CR1=0b01011100;
   //7λ  SPIE  0     ��ֹ��SPRF ��MODF ���жϣ�ʹ����ѯ
   //6λ  SPE   1      SPI ϵͳʹ��
   //5λ  SPTIE  0     ��ֹ��SPTEF ���жϣ�ʹ����ѯ)
   //4λ   MSTR  1      SPI ģ������Ϊ��SPI ����
   //3BIT  CPOL  1      SPI ʱ�ӵ���Ч�����ø�̬��
   //2BIT  CPHA  1        SPSCK �ϵĵ�һ�����س�����8 �������ݴ���ĵ�һ�����ڵ����
   //1BIT  SSOE  1     ��ѡ�����ʹ�ܣ���λ��ʹ�ý��SPCR2 �е�ģʽ����ʹ�ܣ�MODFEN��λ�����ӣ�MSTR�� ����λ����ȷ
                        //��SS �ܽŵĹ���
    /*                    MODFEN SSOE             ��ģʽ                   ��ģʽ
                         0 0                   ͨ��I/O ����SPI��      ��ѡ������
                         0 1                   ͨ��I/O ����SPI��       ��ѡ������
                         1 0                  ģʽ���ϵ�SS           �����ѡ������
                         1 1                     �Զ�SS ���            ��ѡ������          */
   //0bit  LSBFE   1     SPI �������ݴ���ʼ�����λ
   SPI1CR2=0x00;
  //7BIT   0
  //6BIT   XFRW         8λ����16λ
  //5BIT   0
  //4BIT  MODFEN   0    ��ģʽ���Ϲ���ʹ��:  0 ģʽ���Ϲ��ܽ�ֹ����SS �ܽŻָ�Ϊ����SPI ���Ƶ�ͨ��I/O
  //3BIT  BIDIROE  1   ˫��ģʽ���ʹ��
  //2BIT   0
  //1BIT   SPISWAI 0     0 �ڵȴ�ģʽ��SPI ʱ�Ӽ�������
  //0BIT   SPC0    0     0 SPI Ϊ����������������ʹ�ö����ܽ�
   
    SPI1BR=0x71;            //1MHz
 
  //7BIT   0
  //6-4BIT  SPPR[2:0]   SPI ������Ԥ��Ƶϵ�� �� ��3 λ�ֶ�ΪSPI ������Ԥ��Ƶ��ѡ��8 ��ϵ���е�һ������ Table 14-5 ��ʾ����Ԥ
                   //Ƶ������������������ʱ�� ��BUSCLK������Ԥ��Ƶ�����������SPI ������ϵ�������루
                     /*     SPPR2:SPPR1:SPPR0 Ԥ��Ƶ��ϵ��
                                 0:0:0           1
                                 0:0:1           2
                                 0:1:0           3
                                 0:1:1           4
                                 1:0:0           5
                                 1:0:1           6
                                 1:1:0           7
                                 1:1:1           8                */
   //3-0BIT  SPR[3:0]   SPI ������ϵ��
                        /*        SPR3:SPR2:SPR1:SPR0 ����ϵ��
                                      0:0:0:0 2
                                      0:0:0:1 4
                                      0:0:1:0 8
                                      0:0:1:1 16
                                      0:1:0:0 32
                                      0:1:0:1 64
                                       0:1:1:0 128
                                      0:1:1:1 256
                                      1:0:0:0 512
                                All other combinations 512                 */
                                
  
 // SPIS=0x20;
 /* 7   6   5     4  3 2 1 0
 SPRF 0 SPTEF MODF 0 0 0 0
 
 SPRF  0 �������ݻ������������ݿ���
 SPTEF       1 SPI ���ͻ�������
 MODF        ��ģʽ���ϱ��        */
 
  SS2 = 1;
}
                           
/*************************************************************/
/*                       SPI���ͺ���                         */
/*************************************************************/
void SPI1_Write(uint8 data) 
{
  while(!SPI1SR_SPTEF);                // ÿ��ֻ�ܷ���һ���ֽڵ����ݣ�
  SPI1DRL = data;
  
}

/*************************************************************/
/*                       SPI���պ���                         */
/*************************************************************/

 
uint8 SPI1_read(void) 
{
  uint8 temp2,returndata;
 
   while(!SPI1SR_SPIF) 
  temp2 = SPI1SR;   
  returndata = SPI1DRL;           //��ȡ����
  
  return(returndata); 
}     
 