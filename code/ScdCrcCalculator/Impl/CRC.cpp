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
  * @brief    set_parameters简易说明
  * 
  * 设置基本参数模型，如果使用自定义的参数，调用该函数进行修改；
  * 
  * @param    unlPoly    生成项
  * @param    unlInit    寄存器初始化值
  * @param    bRefIn     待测值是否颠倒
  * @param    bRefOut    计算值是否颠倒
  * @param    unlXorOut  输出数据异或项
  * @return   void
  * @author   修黎明 
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
  * @brief    make_crc32_table简易说明
  * 
  * 首先生成crc-32直接查询表，根据crc校验的原理实现；
  * 对0-255进行模2除法生成crc-32查询表
  *
  * @return   void
  * @author   修黎明 
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
  * @brief    make_crc简易说明
  * 
  * 生成CRC校验码
  * 
  * @param    crc      CRC校验码初始值
  * @param    string   待测数据输入项
  * @param    size     待测数据元素个数
  * @return   uint32_t CRC校验码
  * @author   修黎明 
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
			// 将源数据每个字节“颠倒”【RefIn = true】
			chtemp = (unsigned char)reflect(chtemp, 8);
		}
		/*crc = (crc >> 8)^(crc32_table[(crc ^ chtemp)&0xff]); */

		crc = ( crc << 8 ) ^ m_crc32_table[( crc >> 24 ) ^ chtemp];
	}
	if (m_bRefOut)
	{
		// 计算结束后，寄存器中的值“颠倒”【RefOut = true】
		crc = reflect(crc, 32);
	}
	return (crc^m_unlXorOut);  
} 
/**
  * @brief    reflect简易说明
  * 
  * 比特颠倒（即BIT7作为最低位，BIT0作为最高位）
  * 
  * @param    ref     待颠倒项
  * @param    ch      待颠倒项的比特位数
  * @return   int     颠倒后的数据
  * @author   修黎明 
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
  * @brief    generate_crc简易说明
  * 
  * 校验码的计算
  * 
  * @param    spath   待校验文件
  * @return   char    16进制的校验码
  * @author   修黎明 
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


