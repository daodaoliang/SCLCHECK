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
 
	// ֱ�Ӳ�ѯ��
	uint32_t m_crc32_table[256]; 
	// ������
	unsigned long m_unlPloy;
	// �Ĵ�����ʼ��ֵ
	unsigned long m_unlInit;
	// ��������ÿ���ֽ��Ƿ�Ԥ�ȡ��ߵ���
	bool m_bRefIn;
	// ���������Ĵ�����ֵ�ǽ��С��ߵ���
	bool m_bRefOut;
	// ������ݵ������
	unsigned long m_unlXorOut;
	// ����ֵ
	char m_chCRC[20];
public:
	// ���û�������ģ��
	void set_parameters(unsigned long unlPloy,
		                unsigned long unlInit, 
						bool          bRefIn, 
						bool          bRefOut,
						unsigned long unlXorOut);
	// ����CRC-32ֱ�Ӳ�ѯ��
	void make_crc32_table();
	// ����CRC
	uint32_t make_crc(uint32_t crc, unsigned char *string, uint32_t size);
	// ���صߵ�
	unsigned long int reflect(unsigned long int ref, char ch);
	// ����ָ�����ݵ�CRCУ����
	char*  generate_crc(const char* spath,char result[]);

	char*  generate_crc(unsigned char* buf,long lsize,char result[]);
};

