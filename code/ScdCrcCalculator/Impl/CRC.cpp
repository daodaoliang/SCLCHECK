#include "CRC.h"

CCRC::CCRC(void)
{
	m_unlPloy  = 0x04C11DB7;
	m_unlInit = 0xFFFFFFFF;
	m_bRefIn = true;
	m_bRefOut = true;
	m_unlXorOut = 0xFFFFFFFF;

	make_crc32_table();
}


CCRC::~CCRC(void)
{
}
/**
  * @brief    set_parameters����˵��
  * 
  * ���û�������ģ�ͣ����ʹ���Զ���Ĳ��������øú��������޸ģ�
  * 
  * @param    unlPoly    ������
  * @param    unlInit    �Ĵ�����ʼ��ֵ
  * @param    bRefIn     ����ֵ�Ƿ�ߵ�
  * @param    bRefOut    ����ֵ�Ƿ�ߵ�
  * @param    unlXorOut  ������������
  * @return   void
  * @author   ������ 
  * @date     2013/07/14
  */
void CCRC::set_parameters(unsigned long unlPloy,
	                      unsigned long unlInit, 
						  bool          bRefIn,
						  bool          bRefOut, 
						  unsigned long unlXorOut)
{
	m_unlPloy = unlPloy;
	m_unlInit = unlInit;
	m_bRefIn = bRefIn;
	m_bRefOut = bRefOut;
	m_unlXorOut = unlXorOut;

}
/**
  * @brief    make_crc32_table����˵��
  * 
  * ��������crc-32ֱ�Ӳ�ѯ������crcУ���ԭ��ʵ�֣�
  * ��0-255����ģ2��������crc-32��ѯ��
  *
  * @return   void
  * @author   ������ 
  * @date     2013/07/14
  */
void CCRC::make_crc32_table()
{
	unsigned long i32, j32; 
	unsigned long nData32; 
	unsigned long nAccum32; 
	for ( i32 = 0; i32 < 256; i32++ ) 
	{ 
		nData32 = ( unsigned long )( i32 << 24 ); 
		nAccum32 = 0; 
		for ( j32 = 0; j32 < 8; j32++ ) 
		{ 
			if ( ( nData32 ^ nAccum32 ) & 0x80000000 ) 
				nAccum32 = ( nAccum32 << 1 ) ^ m_unlPloy; 
			else 
				nAccum32 <<= 1; 
			nData32 <<= 1; 
		} 
		m_crc32_table[i32] = nAccum32;
	}
}
/**
  * @brief    make_crc����˵��
  * 
  * ����CRCУ����
  * 
  * @param    crc      CRCУ�����ʼֵ
  * @param    string   ��������������
  * @param    size     ��������Ԫ�ظ���
  * @return   uint32_t CRCУ����
  * @author   ������ 
  * @date     2013/07/14
  */
uint32_t CCRC::make_crc(uint32_t crc, unsigned char *string, uint32_t size)  
{  	
	unsigned char chtemp;
	unsigned long ii = 0;

	crc = m_unlInit;

	while(size--)  
	{
		chtemp = *string++;
		if (m_bRefIn)
		{
			// ��Դ����ÿ���ֽڡ��ߵ�����RefIn = true��
			chtemp = (unsigned char)reflect(chtemp, 8);
		}
		/*crc = (crc >> 8)^(crc32_table[(crc ^ chtemp)&0xff]); */

		crc = ( crc << 8 ) ^ m_crc32_table[( crc >> 24 ) ^ chtemp];
	}
	if (m_bRefOut)
	{
		// ��������󣬼Ĵ����е�ֵ���ߵ�����RefOut = true��
		crc = reflect(crc, 32);
	}
	return (crc^m_unlXorOut);  
} 
/**
  * @brief    reflect����˵��
  * 
  * ���صߵ�����BIT7��Ϊ���λ��BIT0��Ϊ���λ��
  * 
  * @param    ref     ���ߵ���
  * @param    ch      ���ߵ���ı���λ��
  * @return   int     �ߵ��������
  * @author   ������ 
  * @date     2013/07/14
  */
unsigned long int CCRC::reflect(unsigned long int ref, char ch)
{
	unsigned long int value = 0;
	for (int i = 1; i < (ch + 1); i ++)
	{
		if (ref&1)
		{
			value |= 1<<(ch - i);
		}
		ref >>= 1;
	}
	return value;
}



/**
  * @brief    generate_crc����˵��
  * 
  * У����ļ���
  * 
  * @param    spath   ��У���ļ�
  * @return   char    16���Ƶ�У����
  * @author   ������ 
  * @date     2013/07/14
  */

 char*  CCRC::generate_crc(const char* spath,char result[])
{  
	make_crc32_table();  

	FILE* sp = fopen(spath, "rb");  	
	if(sp == NULL) return "";

	fseek(sp,0,SEEK_END);
	long lSize = ftell(sp);
	fseek(sp,0,SEEK_SET);
	unsigned char* buf= new unsigned char[lSize];
	memset(buf,0,lSize);  
	fread(buf,1,lSize,sp); 
	fclose(sp);

	sprintf(result,"%08X",make_crc(0,buf,lSize));
	delete buf;

	return result;
}

 char*  CCRC::generate_crc(unsigned char* buf,long lsize,char result[])
 {  
	 make_crc32_table();  

	 sprintf(result,"%08X",make_crc(0,buf,lsize));
	
	 return result;
 }


