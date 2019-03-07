/*=======================================================================
 *Subsystem:   Bootloader
 *File:        main.C
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
#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */
#include "Types.h"
#include "Config.h"
#include "CAN.h"
#include "Errors.h"
#include "PFlash.h"
#include "Srec.h"
#include "PartID.h"
#include "eeprom.h"
#include "Srec.h"
#include "PIT.h"

#pragma MESSAGE DISABLE C5909
#pragma MESSAGE DISABLE C12056

#define RESET_VEC_SRC       0x7FFFE0        //0x7FFFE0
#define RESET_VEC_DST       0x7FDFF8        //0x7FEFF8

#define CPU12IVBR           0xFF00          //bootloader����ַ
#define BUS_CLOCK           32000000

#define __SEG_START_REF(a)  __SEG_START_ ## a
#define __SEG_END_REF(a)    __SEG_END_ ## a
#define __SEG_SIZE_REF(a)   __SEG_SIZE_ ## a
#define __SEG_START_DEF(a)  extern INT8 __SEG_START_REF (a) []
#define __SEG_END_DEF(a)    extern INT8 __SEG_END_REF   (a) []
#define __SEG_SIZE_DEF(a)   extern INT8 __SEG_SIZE_REF  (a) []

__SEG_START_DEF (RAM_CODE);
__SEG_END_DEF   (RAM_CODE);
__SEG_SIZE_DEF  (RAM_CODE);

volatile  ErrType FlashErr;
UINT16 *boot;       //����BootLoader��ȷ���ִ洢��ַ
UINT16 *DataIVBR;   //�Ƿ���Ҫ������ȷ���ִ洢��ַ
/*=======================================================================
 *������:      static UINT8 EraseFlash_Store(void)
 *����:        ����Flash
 *����:        ��       
 *���أ�       ��
 *˵����       �ڴ���洢�Ĺ����в����洢����flash�ռ䣬
               ����洢������Ϊ��0x70_0000~0x77_0000
========================================================================*/
static UINT8 EraseFlash_Store(void)
{
  UINT8 Error;
  
  switch (PARTID)
  {
    //S12XE 1024k flash
    case MASK_0M48H:  //Falling to next case
    case MASK_1M48H:  //Falling to next case
    case MASK_2M48H:  //Falling to next case
    case MASK_3M48H:  //Falling to next case
    case MASK_4M48H_5M48H:
    case MASK_0N35H_1N35H:
      if(Error = PFlash_EraseBlock(0x740000UL))
      {
        return(Error);
      }
      if(Error = PFlash_EraseBlock(0x700000UL))
      {
        return(Error);
      }
      return(noErr);
    
    default:
      return(UnknownPartID);
  }      
}

/*=======================================================================
 *������:      static UINT8 EraseFlash_Upgrade(void)
 *����:        ����Flash
 *����:        ��       
 *���أ�       ��
 *˵����       �ڴ��������Ĺ����в���Դ����洢����flash�ռ䣬
               ����洢������Ϊ��0x78_0000~0x7FDFDF
               �˴�Ϊ�������
========================================================================*/
static UINT8 EraseFlash_Upgrade(void)
{
  UINT8 Error;
  
  switch (PARTID)
  {
    //S12XE 1024k flash
    case MASK_0M48H:  //Falling to next case
    case MASK_1M48H:  //Falling to next case
    case MASK_2M48H:  //Falling to next case
    case MASK_3M48H:  //Falling to next case
    case MASK_4M48H_5M48H:
    case MASK_0N35H_1N35H:
      //erase whole P-Flash block 0 except bootloader area
      if(Error = PFlash_EraseSectorBySector(0x7C0000UL, 0x7FDFDFUL))
      {
        return(Error); 
      }
      //and erase all remaining blocks
      if(Error = PFlash_EraseBlock(0x7A0000UL))
      {
        return(Error);  
      }
      if(Error = PFlash_EraseBlock(0x780000UL))
      {
        return(Error); 
      }
      return(noErr);
    
    default:
      return(UnknownPartID);
  }      
}

/*=======================================================================
 *������:      static UINT8 ProgramFlash_Upgrade(void)
 *����:        ����д��Flash
 *����:        ��       
 *���أ�       ��
 *˵����       �ڴ�������������н��洢��(0x70_0000~0x77_0000)�Ĵ���д��Դ��������
               �˴�Ϊ������
========================================================================*/
static UINT8 ProgramFlash_Upgrade(void)
{
  SRecDataRec ProgSRec;
  UINT8 Error = noErr; 
  
  for(;;)
  {
    //go get an S-Record, return if there was an error
    if (Error = RcvSRecord_Upgrade(&ProgSRec)) 
    {
      return(0x81);
    }
    
    __asm nop;
    SrcdAlign4W(&ProgSRec);
    __asm nop;
    
    if (ProgSRec.RecType == EndRec) // S7, S* or S9 record?
    {
      break;                       // yes. return
    }
      
    else if (ProgSRec.RecType == HeaderRec) //S0 record?
      continue;             //yes. just ignore it
      
    else                    //a data record was received
    {
      if (ProgSRec.LoadAddr == RESET_VEC_SRC)//��λ�����ĵ�ַ
      {
        if(Error = PFlash_Program(RESET_VEC_DST, (UINT16 *)&ProgSRec.Data[24]))
        {
          return(0x82);
        }
      }   
      else
      {
        switch (PARTID)
        {       
          //S12XE 1024k flash
          case MASK_0M48H:  //Falling to next case
          case MASK_1M48H:  //Falling to next case
          case MASK_2M48H:  //Falling to next case
          case MASK_3M48H:  //Falling to next case
          case MASK_4M48H_5M48H:
          case MASK_0N35H_1N35H:
            if(Error = PFlash_Program(ProgSRec.LoadAddr, (UINT16 *)&ProgSRec.Data))
              return(0x83);
        
            if(Error = PFlash_Program(ProgSRec.LoadAddr+8, (UINT16 *)&ProgSRec.Data[8]))
              return(0x83);
      
            if(Error = PFlash_Program(ProgSRec.LoadAddr+16, (UINT16 *)&ProgSRec.Data[16]))
              return(0x83);
      
            if(Error = PFlash_Program(ProgSRec.LoadAddr+24, (UINT16 *)&ProgSRec.Data[24])) 
              return(0x83);
              break;
              
          default:
            return(UnknownPartID);
        } 
      }
      CAN_SendByte(0x02);
    }
  }
  return(Error);
}

/*=======================================================================
 *������:      static UINT8 ProgramFlash_Store(void)
 *����:        Զ�̽�����洢��pFlash(0x70_0000~0x77_0000)��
 *����:        ��       
 *���أ�       ��
 *˵����       Զ�̽�����洢��pFlash(0x70_0000~0x77_0000)��
               �˴�Ϊ����洢
========================================================================*/
static RecData_T RecData;
static UINT8 ProgramFlash_Store(void)
{
  SRecDataRec ProgSRec;
  UINT8 Error = noErr; 
  UINT32 LoadAddr = PFLASH_START; 
  RecData_T*dataptr = &RecData;
  UINT8 addrcount,k,j;
  UINT16 *Address=NULL;
  
  Data_INIT(dataptr);//��ʼ�����ջ���
  for(;;)
  {
    if (Error = RcvSRecord_Store(&ProgSRec,dataptr)) 
    {
      return(0x11);
    } 
      
    if (ProgSRec.RecType == HeaderRec) //S0 record
    {
      continue;
    }
    else                    
    {
      addrcount = (dataptr->Lenth)%8;         //����ÿ��S19�ļ��е��ַ�����(ÿ�δ洢8�ֽ�)      
      Address = (UINT16 *)(&dataptr->DATA[0]);//ÿ��S19�ַ����׵�ַ
      switch (PARTID)
      {       
        case MASK_0M48H:  //Falling to next case
        case MASK_1M48H:  //Falling to next case
        case MASK_2M48H:  //Falling to next case
        case MASK_3M48H:  //Falling to next case
        case MASK_4M48H_5M48H:
        case MASK_0N35H_1N35H:
            if(addrcount != 0)//ÿ��S19���ֽ�����8������(ÿ�η��ʹ洢8�ֽ�)
            {
               for(k=0; k<8-addrcount; k++)//������8�ֽڵİ�����0xFF
               {
                 if(Data_IN(dataptr,0xFF) == 1)//��0xFF
                 {
                    return (0x12); 
                 }
               }
               addrcount = (dataptr->Lenth-1)/8;//����ÿ��S19���ݷ��͵Ĵ���(ÿ��8�ֽ�)
               for(k=0; k<addrcount+1; k++)
               {
                  if(Error = PFlash_Program(LoadAddr, Address)) //��Address�е����ݴ洢��flash��ַ(LoadAddr)��
                  {
                    return(0x13); 
                  }
                  Address =  Address + 4;  //ÿ�δ洢2���ֽ�(AddressΪUINT16��ָ��)
                  LoadAddr = LoadAddr + 8; //flash��ַÿ�μ�8
                  for(j=0; j<8; j++)
                  {
                    Data_OUT(dataptr);     //ÿ��ȡ��8�ֽ�
                  }
               }
            }
            else//ÿ��S19���ֽ�����8������
            {
               addrcount = (dataptr->Lenth-1)/8; //���ʹ���
               for(k=0; k<addrcount+1; k++)
               {
                  if(Error = PFlash_Program(LoadAddr, Address))
                  {
                    return(0x14); 
                  }
                  Address =  Address + 4;   //ÿ�δ洢2���ֽ�(AddressΪUINT16��ָ��)
                  LoadAddr = LoadAddr + 8;  //flash��ַÿ�μ�8 
                  for(j=0; j<8; j++)
                  {
                    Data_OUT(dataptr);      //ÿ��ȡ��8�ֽ�
                  }               
               }
            }
            //Data_INIT(dataptr); //ÿ��S19�ļ�������֮���������(������,�ж��в������ݱ����)
            break;
              
        default:
          return(UnknownPartID);
         
      }
      if (ProgSRec.RecType == EndRec)         
      {
         return (noErr);
      }
      CAN_SendByte(0x01);
    }
  }
  return(Error);
}

/*=======================================================================
 *������:      static void CopyCodeToRAM(void)
 *����:        ��flash���ж�д�ĳ�����Ҫ���临����RAM�н�������
 *����:        ��       
 *���أ�       ��
 *˵����       ������RAM����ʼ��ַ������Prm�ļ��е�relocate_to��ַһ��
               ������relocate_to�ͱ���ʹ�ô˶δ���,����ϵͳ����
========================================================================*/
static void CopyCodeToRAM(void)           
{
  UINT8 *Src;
  UINT8 *Dst;
  UINT16  SegSize;
  UINT16  x;
  
  //RAM code resides in Flash from 0xfc00 - 0xfeff
  Src = (UINT8 *)__SEG_START_REF(RAM_CODE);
  //code will be copied to RAM at address 0x3C00
  Dst = (UINT8 *)0x3C00;//һ��Ҫ��Prm�ļ��е�relocate_to��ַһ��
  SegSize = (UINT16)__SEG_SIZE_REF(RAM_CODE);
  
  for (x = 0; x < SegSize; x++)   //just copy a byte at a time
  {
    *Dst++ = *Src++; 
  }
}

/*=======================================================================
 *������:      static void InterruptModuleSetup(void)
 *����:        ����BootLoader�����е��жϻ�������ַ
 *����:        ��       
 *���أ�       ��
 *˵����       �˻���ַΪBootLoader���ж���������ַ
 
========================================================================*/
static void InterruptModuleSetup(void)
{
  IVBR = (CPU12IVBR >> 8);
}

/*=======================================================================
 *������:      static void PLL_Init(void)
 *����:        ��ʼ�����໷
 *����:        ��       
 *���أ�       ��
 *˵����       ����Ƶ��:16MHz
               BusƵ��:32MHz 
========================================================================*/
static void PLL_Init(void) 
{
    CLKSEL &= 0x7f;       //����OSCCLK��Ϊϵͳʱ��
    PLLCTL &= 0x8F;       //��ֹ���໷
    CRGINT &= 0xDF;
    //PLLCLK=2��OSCCLK��(SYNR+1)/(REFDV+1), fbus=PLLCLK/2
    #if(BUS_CLOCK == 120000000) 
        SYNR = 0xcd;
    #elif(BUS_CLOCK == 104000000) 
      SYNR = 0xcc;
    #elif(BUS_CLOCK == 96000000) 
      SYNR = 0xcb;
    #elif(BUS_CLOCK == 88000000) 
      SYNR = 0xca;
    #elif(BUS_CLOCK == 80000000) 
      SYNR = 0xc9;
    #elif(BUS_CLOCK == 72000000) 
      SYNR = 0xc8;
    #elif(BUS_CLOCK == 64000000) 
      SYNR = 0xc7;
    #elif(BUS_CLOCK == 56000000) 
      SYNR = 0xc6;
    #elif(BUS_CLOCK == 48000000) 
      SYNR = 0xc5;
    #elif(BUS_CLOCK == 40000000) 
      SYNR = 0x44;
    #elif(BUS_CLOCK == 32000000)
      SYNR = 0x43;     
    #elif(BUS_CLOCK == 24000000)
      SYNR = 0x42;
    #elif(BUS_CLOCK == 16000000)
      SYNR = 0x01;
   #endif 

    REFDV = 0x81;
    PLLCTL |=0x70;         //ʹ�����໷
    asm NOP;
    asm NOP;
    while(!(CRGFLG&0x08)); //PLLCLK����
    CLKSEL |= 0x80;        //����PLLCLKΪϵͳʱ��
}


/*=======================================================================
 *������:      static void EraseFlash_Status(void)
 *����:        ִ�в����洢�����ǲ��������� 
 *����:        data��CAN��������
               fun���ص�����        
 *���أ�       ��
 *˵����       ִ�в����洢�����ǲ���������                 
========================================================================*/
static void EraseFlash_Status(UINT8 data, UINT8 (*fun)(void))
{
   UINT8 flashsta=0;
   UINT8 i,j;
   flashsta = fun();
   
   if(flashsta != NoErr)//��������д�,���ͳ�����Ϣ
   {
     while(1)
     {
       CAN_SendByte(data);
       for(i=0; i<100; i++)
       {
         for(j=0; j<200; j++);
       }
     }
   }
}
/*=======================================================================
 *������:      static Boot_Delay (void)
 *����:        Boot����ʱ����
 *����:        ts:��ʱ�Ľ�����       
 *���أ�       ��
 *˵����       ����Ƶ��:16MHz
               BusƵ��:32MHz 
========================================================================*/
static
void Boot_Delay(unsigned int ts)
{
   unsigned int i,j;
   for(i=0; i<500; i++)
   {
      for(j=0; j<ts; j++);
   }
}

/*=======================================================================
 *������:      static void PLL_Init(void)
 *����:        ��ʼ�����໷
 *����:        ��       
 *���أ�       ��
 *˵����       ����Ƶ��:16MHz
               BusƵ��:32MHz 
========================================================================*/
UINT16 QQ[5]={0,0,0,0,0};//���Ա��
UINT32 Flashstart_Add;
//UINT16 dd;
void main(void) 
{
  UINT8 caseflag=0,i,j;
  UINT8 flashsta=0,state;
  //UINT16 datast;
  memset(QQ, 0, 5*sizeof(UINT16));

  DisableInterrupts;
  /*������Ҫ����CAN���շ�������(�����߹���)
  DDRS_DDRS2 = 1;//ʹ��CAN�շ���
  PTS_PTS2 = 0; 
  
  DDRK_DDRK5 = 0;//����CAN�ĵ�Դ 
  PORTK_PK5 = 1;*/    
  
  Flashstart_Add = PFLASH_START; //boot��ȫ�ֱ��������ڳ�ʼ��������,��������
  
  //��Ƭ����ʼ��
  PLL_Init(); //BusClk:32MHz  
  INIT_CAN2();//CAN3��ʼ��,������:500kbps
  
  CopyCodeToRAM();//����flash��д�Ĵ��븴����RAM������
  InterruptModuleSetup();//����Boot���ж���������ַ
  //��Pflash��Dflash�ĳ�ʼ��
  Init_Eeprom(&FlashErr); //�趨flash��ʱ��
  //���ַ���(��Ƭ�����״�ʹ��ʱ��ý���ȫ�����Ͳ���flash���еĿ�)
  FlashErr = LaunchFlashCommand(2 ,PARTITION_D_FLASH, 0, DFPART, EEE_RAM, 0, 0, 0, 0, 0); 
  ErrorCheck(FlashErr,(accerr|fpviol|mgstat1|mgstat0), (erserif|pgmerif|epviolif|ersvif1|ersvif0|dfdif|sfdif));
  if(FlashErr.ErrCode == NoErr) 
  {
    FlashErr = LaunchFlashCommand(0 ,EEPROM_QUERY, 0, 0, 0, 0, 0, 0, 0, 0); 
    ErrorCheck(FlashErr,(accerr|fpviol|mgstat1|mgstat0), (erserif|pgmerif|epviolif|ersvif1|ersvif0|dfdif|sfdif));
    ChecPartErr(&FlashErr);   
  }  
  if(FlashErr.ErrCode == NoErr) //�޴���ʹ��EEPROM
  {
    FlashErr = LaunchFlashCommand(0 ,ENABLE_EEPROM_EMULATION, 0, 0, 0, 0, 0, 0, 0, 0);//BuffRamģ��EEprom(EEE)
    ErrorCheck(FlashErr, (accerr|fpviol|mgstat1|mgstat0), (erserif|pgmerif|epviolif|ersvif1|ersvif0|dfdif|sfdif));    
  }
  CAN_SendByte(0x55);
  //EEPROM��ֵ��ʼ��
  boot = (UINT16*)0x0C00;
  DataIVBR = (UINT16*)0x0C02;
  (*boot) = 0x66;
  /*
  if((*DataIVBR) == 0x77)//��Ҫ��Ӧ�ó���������һ����ַ��������BOOT������������Ϻ����ԭ�ȵ�AAP��
  {
    (*DataIVBR) = 0x11;
  }
  else if((*DataIVBR) == 0x55)//�������������е���,��ô��������������ֱ�ӽ�����������(����Ҫ����)
  {
    if((*(UINT16*)0xC000) != 0xFFFF) //���APP�洢����������ֱ�ӽ�������
    {
      caseflag = 0;   //caseflag == 1;����Ҫ����S19�ļ�ֱ�ӽ�������
    }
  }*/
  EnableInterrupts;       //enable interrupts for the CAN

  for(;;)
  { 
    switch(caseflag)
    {
      case 0:
        QQ[0]++;
        CAN_SendByte(0xC0);   //����BOOT���򲢿�ʼ���в���
        EraseFlash_Status(0x10, EraseFlash_Store); //�����洢��flash(�˴��������boot������Ҫ���¸���S19�ļ�)
        CAN_SendByte(0xC1);  //Flash�������
        QQ[1]++;
        
        CAN2RIER_RXFIE = 1;   //����CAN2�����ж�  
        if(state = ProgramFlash_Store())
        {
           while(state != 0)
           {
             CAN_SendByte(state);//����洢����,��Ҫ���������¸���Boot����
             for(i=0; i<250; i++)
             {
               for(j=0; j<250; j++);
             }
           }
        }
        CAN2RIER_RXFIE = 0;   //����洢�����ر��ж�
        
        CAN_SendByte(0xC2);
        
        RxIn = RxOut= 0;      //��Ҫ������ջ���,(��ΪTXT�ļ���β����2����β��,��Ҫ�������)
        RxBAvail = RxBufSize; //the receive buffer is empty
        
        (*DataIVBR) = 0x55;   //����洢�������λ,����������λ
        caseflag = 1;
        QQ[2]++;
      break;
      
      case 1:
        CAN_SendByte(0xC3);   //�����������
        EraseFlash_Status(0x80, EraseFlash_Upgrade);//���������flash
        CAN_SendByte(0xC4);   //�����������
        QQ[3]++;
        
        init_PIT0();//�ж�Ƶ��:10ms(�˴��ж�δ����,�����������п���)
        if(state = ProgramFlash_Upgrade())
        {
           while(state != 0)
           {
             CAN_SendByte(state);//������������,��Ҫ���������¸���Boot����
             for(i=0; i<250; i++)
             {
               for(j=0; j<250; j++);
             }
           }
        }
        PITINTE_PINTE0 = 0;   //�ض��жϱ��������ȡFlash���б��
        
        (*boot) = 0;          //ֻ�и�����֮����ûر��λ
        (*DataIVBR) = 0x66;   //�˴����λ������Boot����APP����
        
        CAN_SendByte(0xC5);   //�����������
        
        caseflag = 2;
        QQ[4]++;
        //while(1);             //���������ɺ���Ҫ��������
        
        Boot_Delay(1000);
        for(i=0;i<250;i++)
        {
          for(i=0;i<250;i++);  
        }
        COPCTL = 0x01;       //��������     
        ARMCOP = 0x00;
      break;
      
      default:
        CAN_SendByte(0xC6);//������¹���
      break;
    }  
  }   
}
/******************************************************************************/
#pragma CODE_SEG RAM_CODE
interrupt void isrPIT0()//PIT0��ʱ�жϺ���,����Ϊ5ms,ʱ���ٶ̻����(���Դ���5ms)
{  
   UINT8 i;
   UINT16 data;
   if (PITTF_PTF0 ==1) 
   {   
     PITTF_PTF0 = 1;
   } 
   for(i=0; i<16; i++)//�˴����Ϊ16����ѭ��,�Ĵ�ᵼ�¶��д洢���
   {
     data = PFlash_ReadWord(Flashstart_Add);
     if (RxBAvail != 0)            //if there are bytes available in the Rx buffer
     {
        RxBAvail--;
        RxBuff[RxIn++] = (UINT8)(data>>8);//���ֽ�
        RxBAvail--;
        RxBuff[RxIn++] = (UINT8)data;
        if (RxIn == RxBufSize)      //���ֽ�
        { 
          RxIn = 0;               //yes. wrap around to the start
        }                 
     }
     Flashstart_Add = Flashstart_Add + 2; 
   }
}
#pragma CODE_SEG DEFAULT
