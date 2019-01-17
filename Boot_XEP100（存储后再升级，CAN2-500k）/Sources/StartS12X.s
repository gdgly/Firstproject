;********************************************************************************
;* Freescale Semiconductor Inc.
;* (c) Copyright 2004-2005 Freescale Semiconductor, Inc.
;* ALL RIGHTS RESERVED.
;********************************************************************************
;Services performed by FREESCALE in this matter are performed AS IS and without 
;any warranty. CUSTOMER retains the final decision relative to the total design 
;and functionality of the end product. FREESCALE neither guarantees nor will be 
;held liable by CUSTOMER for the success of this project.
;FREESCALE DISCLAIMS ALL WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY INCLUDING, 
;BUT NOT LIMITED TO, IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR 
;A PARTICULAR PURPOSE ON ANY HARDWARE, SOFTWARE OR ADVISE SUPPLIED TO THE PROJECT
;BY FREESCALE, AND OR NAY PRODUCT RESULTING FROM FREESCALE SERVICES. IN NO EVENT
;SHALL FREESCALE BE LIABLE FOR INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
;THIS AGREEMENT.
;
;CUSTOMER agrees to hold FREESCALE harmless against any and all claims demands or 
;actions by anyone on account of any damage, or injury, whether commercial, 
;contractual, or tortuous, rising directly or indirectly as a result of an advise
;or assistance supplied CUSTOMER in connection with product, services or goods 
;supplied under this Agreement.
;********************************************************************************
;* File      StartS12X.s
;* Owner     b01802
;* Version   1.0   
;* Date      Dec-02-2010
;* Classification   General Business Information
;* Brief     Startup file
;********************************************************************************
;* Detailed Description:
;*
;*    Tested on: DEMO9S12XSFAME, EVB9S12XEP100
;*    
;*    The bootloder handles all reset vectors. This startup file makes a decision
;*    whether the bootloader or user application is executed. 
;********************************************************************************
;Revision History:
;Version   Date          Author    Description of Changes
;1.0       Dec-02-2010  b01802    Initial version
;********************************************************************************
;
  xref  main            ;xref��ʾ���ӳ���Ķ����������ļ���
;
  xdef  _BootStart      ;xdef��ʾ�ڱ��ļ��ж����˿��Ա������ļ��еĳ�����õ��ӳ���
;

AppResetVect:   equ     $dffe ;equ���Ѹõ�ַ����AppResetVect��� here is stored reset vector of user application
StackTop:       equ     $3900 ;ջ����ַ
BootCtrWord:    equ     $0C00 ;EEPROM��0C00λ�����ڴ���BootLoader������
APPCtrWord:     equ     $0C02 ;ȷ�����ص�ַ
APPStartUp:     equ     $C000 ;APP��Startup��ַ

_BootStart:
  
  ldd   APPStartUp      
  cpd   #$ffff            
  beq   GoBoot  
  ;��������ʱBDM�����PFLASH��ַ�е�ȫ��ֵ,��APP���޳��������Boot������
  ;�˴�������Ե�BOOT��2������ʱ,��ַ0x0C02�ϴδ洢��ֵΪ0x11����ʱ����ʱ������޳����APP��
  ; -----------------------------------------------------------------------------
  
  ldd   APPCtrWord      
  cpd   #$11            
  beq   GoApp           
  ;˵��:�״�����BOOTʱ,�˵�֮�е�ֵΪ0XFF,Pfalsh����APP����,���������ɹ�,�����һֱ����BOOT��
  ;����APP�����н���BOOT״̬ʱ,Ӧ�ó����н�0x0C02��ַ�е�ֵ��Ϊ0x77,0x0C00��ַ�е�ֵ��Ϊ0x66,
  ;��ʱ�򲻽���APP��,��������ָ��(��:����BOOT��), ��BOOT������ز��ɹ���0x0C02�е�ֵһֱΪ0x11,
  ;��ô��һ�����������������ԭ����APP��,�����سɹ���ô0x0C02��ַ�е�ֵ��Ϊ0x55,�´���������ʱ,
  ;����Ҫ����APP�������ֱ�ӽ�������(������case2��).
  ; -----------------------------------------------------------------------------
  
  ldd   BootCtrWord
  cpd   #$66
  beq   GoBoot         
  ;˵��:���Ӧ�ó�������Ҫ����BOOT��,����Ӧ�ó����н�0x0C00���ֽڸ�Ϊ0x66�ͽ�0x0C02�е�ֵ��Ϊ0x77
  ;��ô����������Ϳ��Խ���BOOT��startup��              
  ; -----------------------------------------------------------------------------
  
  ldd   AppResetVect    
  cpd   #$ffff          
  beq   GoBoot          
  ;˵��:��Ƭ���״�ʹ��ʱFlash�������ԭʼֵΪ0xFF;������������BOOT�ĳ�����                     
  ; -----------------------------------------------------------------------------
 
  ldd   APPCtrWord      
  cpd   #$66            
  beq   GoApp           
  ;˵��:�����ָ��ʱ,˵�����غ������Ѿ��ɹ�������µ�APP������
  ; -----------------------------------------------------------------------------
  
GoBoot:
  lds  #StackTop
  jmp  main                        ;then start the bootloader
  
GoApp:  
  ldx   AppResetVect    ;��App��ַ���ص��ۼ���X��
  jmp    0,x            ;jump to the application
  
;********************************************************************************

