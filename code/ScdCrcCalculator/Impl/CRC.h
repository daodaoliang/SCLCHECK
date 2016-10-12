#pragma once

#include "..\ScdCrcCalculator.h"

#include<stdio.h>   
#include<stdlib.h>   
#include<string.h>   


#define log_debug(pszFormat, ...) 
#define log_info(pszFormat, ...) 
#define log_warn(pszFormat, ...) 
#define log_error(pszFormat, ...)

typedef unsigned int uint32_t;

class SCDCRCCALCULATOR_API CCRC
{
public:
	CCRC(void);
	~CCRC(void);
 
	// 直接查询表
	uint32_t m_crc32_table[256]; 
	// 生成项
	unsigned long m_unlPloy;
	// 寄存器初始化值
	unsigned long m_unlInit;
	// 待测数据每个字节是否预先“颠倒”
	bool m_bRefIn;
	// 计算结束后寄存器的值是进行“颠倒”
	bool m_bRefOut;
	// 输出数据的异或项
	unsigned long m_unlXorOut;
	// 返回值
	char m_chCRC[20];
public:
	// 设置基本参数模型
	void set_parameters(unsigned long unlPloy,
		                unsigned long unlInit, 
						bool          bRefIn, 
						bool          bRefOut,
						unsigned long unlXorOut);
	// 生成CRC-32直接查询表
	void make_crc32_table();
	// 生成CRC
	uint32_t make_crc(uint32_t crc, unsigned char *string, uint32_t size);
	// 比特颠倒
	unsigned long int reflect(unsigned long int ref, char ch);
	// 计算指定数据的CRC校验码
	char*  generate_crc(const char* spath,char result[]);

	char*  generate_crc(unsigned char* buf,long lsize,char result[]);
};

