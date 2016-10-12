#include "ScdCrcCalculatorImpl.h"
#include ".\Impl\SCDCRC.h"
#include "..\ScdCheckCommon\charconv.h"
#include <Shlwapi.h>

LPCSTR CalcCrcImpl(LPCSTR lpFilePath, char szErrorMessage[], int nBufferSize)
{
    // todo: lpFilePath为scd文件路径，此函数加载文件并计算crc列表，将结果组织成合法的xml字符串
    // 返回给调用者。如果中途出现错误，则将错误信息以文本的形式存储在szErrorMessage中，注意字符串
    // 写入时不要超过nBufferSize的限值（含结尾0）。
    // 返回值和错误信息都是utf8编码。
    // 返回值的内存分配使用malloc函数。
    // lpFilePath一定存在，在外部已经做了判断。

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
		lstrcpynA(szErrorMessage, WtoU(L"解析文件失败【未知的错误】").c_str(), nBufferSize);
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
		lstrcpynA(szErrorMessage, WtoU(L"内存分配失败").c_str(), nBufferSize);
		return NULL;
	}
    lstrcpynA(pBuffer, sXmlUtf8.c_str(), lBufferSize);

	return pBuffer;
}
