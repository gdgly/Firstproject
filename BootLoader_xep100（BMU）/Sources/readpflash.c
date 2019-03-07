#define PFLASH_START               0x00700000

#define PFLASH_PAGE_SIZE           0x4000
#define PFLASH_PAGE_START          0xC0
#define PFLASH_PAGE_WINDOW_START   0x8000
#define ReadWord(address)          ((UINT16*)(*(volatile UINT16 *__near)(address))

UINT16 PFlash_ReadWord (UINT16 destination) //��ȡ2���ֽ�
{
  UINT8   lastepage;    //���ڴ洢PPAGE��ֵ
  UINT8   p_page;       //���ڼ���PPAGE��ֵ
  UINT16  data;         //��ȡ��������
  
  lastepage = PPAGE;   //����EPAGE��ֵ
  p_page = (UINT8)(PFLASH_PAGE_START + (destination >> 14));   //����PPAGE
  PPAGE = p_page;                                              //��PPAGE��ֵ

  data = ReadWord((destination & (PFLASH_PAGE_SIZE - 1)) + PFLASH_PAGE_WINDOW_START);  //��ȡҳ�洰���е�����
  EPAGE= lastepage;       //�ָ�PPAGE��ֵ
  return(data);
}


UINT16 PFlash_ReadAll (void)
{
  
}