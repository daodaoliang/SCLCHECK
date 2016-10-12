#include "ScdCrcCalculatorImpl.h"
#include ".\Impl\SCDCRC.h"
#include "..\ScdCheckCommon\charconv.h"
#include <Shlwapi.h>

LPCSTR CalcCrcImpl(LPCSTR lpFilePath, char szErrorMessage[], int nBufferSize)
{
    // todo: lpFilePathΪscd�ļ�·�����˺��������ļ�������crc�б��������֯�ɺϷ���xml�ַ���
    // ���ظ������ߡ������;���ִ����򽫴�����Ϣ���ı�����ʽ�洢��szErrorMessage�У�ע���ַ���
    // д��ʱ��Ҫ����nBufferSize����ֵ������β0����
    // ����ֵ�ʹ�����Ϣ����utf8���롣
    // ����ֵ���ڴ����ʹ��malloc������
    // lpFilePathһ�����ڣ����ⲿ�Ѿ������жϡ�

    // custom code goes here:	
	CSCDCRC scd;
	std::string sfile = lpFilePath;	
	bool bScdLoaded = false;	

	try
	{
		bScdLoaded = scd.Load(AtoW(sfile).c_str());
	}
	catch(std::runtime_error& err)
	{
		lstrcpynA(szErrorMessage, AtoU(err.what()).c_str(), nBufferSize);
	}	
	catch(...)
	{
		lstrcpynA(szErrorMessage, WtoU(L"�����ļ�ʧ�ܡ�δ֪�Ĵ���").c_str(), nBufferSize);
	}
	
	if (!bScdLoaded) 
	{
		return NULL;
	}
	
	CrcCodeVec& vecCrc = scd.m_IedCrcCodeVec;
	int nIedCount = vecCrc.size();

	CMarkup xml;
	xml.SetDoc(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");

	xml.AddElem(L"SCD");
	xml.IntoElem();

	for (int iIed = 0; iIed < nIedCount; iIed++)
	{		
		wCrcCode& crc = vecCrc[iIed];

        xml.AddElem(L"IED");
        xml.IntoElem();

		xml.AddElem(L"NAME");
		xml.SetElemContent(crc.sIedName);
		xml.AddElem(L"DESC");
		xml.SetElemContent(crc.sIedDesc);
		xml.AddElem(L"MANU");
		xml.SetElemContent(crc.sIedManu);
		xml.AddElem(L"TYPE");
		xml.SetElemContent(crc.sIedType);
		xml.AddElem(L"CRC");
		xml.SetElemContent(crc.sCrc);
		xml.AddElem(L"CRCINFILE");
		xml.SetElemContent(crc.sCrcInFile);
		
		xml.OutOfElem();
	}	
	xml.OutOfElem();

	std::string  sXmlUtf8 = WtoU(xml.GetDoc());
	long lBufferSize = sXmlUtf8.size() + 1;
	char* pBuffer = (char*)malloc(lBufferSize);
	if (pBuffer == NULL) 
	{
		lstrcpynA(szErrorMessage, WtoU(L"�ڴ����ʧ��").c_str(), nBufferSize);
		return NULL;
	}
    lstrcpynA(pBuffer, sXmlUtf8.c_str(), lBufferSize);

	return pBuffer;
}
