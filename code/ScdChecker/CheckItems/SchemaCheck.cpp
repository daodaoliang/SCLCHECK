#include "SchemaCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>
#include <Shlwapi.h>
#include <atlstr.h>
#include "../../ScdCheckCommon/OperHandler.h"
#include <Windows.h>

using namespace std;
using namespace com_ximpleware;

#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib, "../lib/x64/xerces-c_3D.lib")
#else
#pragma comment(lib, "../lib/win32/xerces-c_3D.lib")
#endif
#else
#ifdef _WIN64
#pragma comment(lib, "../lib/x64/xerces-c_3.lib")
#else
#pragma comment(lib, "../lib/win32/xerces-c_3.lib")
#endif
#endif

#define KEY "DB5780"

CSchemaCheck::CSchemaCheck()
{
    CheckItemConfig cfg;

    // ȡ����Ŀ��������Ϣ�����õ�������
    if (CConfigMgr::GetInstance()->GetItemConfig(KEY, cfg))
    {
        SetConfig(cfg);
    }

    // ��������˱���Ŀ����������ע��
    if (IsEnabled())
    {
        CScdChecker::GetInstance()->RegisterCheckItem(this);

        // �ڴ˴������Ҫĳĳ���͵Ļ�����Ϣ�������Լ��������Ŀ
        InitPaths();
    }
}
void CSchemaCheck::InitPaths()
{
    char szPath[MAX_PATH];
    const char *lpAddtions[] = {
        "\\..\\xsd\\SCL.xsd",
        "\\..\\..\\xsd\\SCL.xsd",
        "\\..\\..\\Config\\xsd\\SCL.xsd",
    };
    vector<string>().swap(m_vectorFilePaths);
    for (int i = 0; i < RTL_NUMBER_OF(lpAddtions); ++i)
    {
        GetModuleFileNameA(GetModuleHandle(NULL), szPath, RTL_NUMBER_OF(szPath));
        PathAppendA(szPath, lpAddtions[i]);
        m_vectorFilePaths.push_back(szPath);
    }
    for (int i = 0; i < m_vectorFilePaths.size(); i++)
    {
        try
        {
            FILE *fp = fopen(m_vectorFilePaths[i].c_str(), "r");
            if (fp != NULL)
            {
                xsdPath = AtoW(m_vectorFilePaths[i]);
                fclose(fp);
                fp = NULL;
                return ;
            }
        }catch(...)
        {
        }
    }
    return ;
}

static HANDLE g_hInitializeMutex = CreateMutex(NULL, FALSE, NULL);

bool CSchemaCheck::CheckInternal()
{
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    // ���������
    COutputMgr *pOutputMgr = COutputMgr::GetInstance();
    // �����ļ�������
    CConfigMgr *pConfigMgr = CConfigMgr::GetInstance();

    assert(pChecker != NULL);
    assert(pCacheMgr != NULL);
    assert(pOutputMgr != NULL);
    assert(pConfigMgr != NULL);

    // custom code goes here
    class ErrorHandle : public OperErrorHandler
    {
        virtual void Report(const SAXParseException& exc)
        {
            int iCol = exc.getColumnNumber();
            int iRow = exc.getLineNumber();
            wstring sMes(exc.getMessage());
            string xml = CScdChecker::GetInstance()->GetRowContentByRowNumber(iRow);
            CSchemaCheck::GetInstance()->Report(iRow, xml, WtoA(sMes).c_str());
        }
    };
    const ErrorHandle Errorhandler;
    
    try
    {
        WaitForSingleObject(g_hInitializeMutex, INFINITE);

        XMLPlatformUtils::Initialize();
    }
    catch (...) {
        ReleaseMutex(g_hInitializeMutex);
        return false;
    }

    ReleaseMutex(g_hInitializeMutex);

    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
    parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    // ����У�鹦��
    parser->setFeature(XMLUni::fgXercesSchema, true);
    // ��У���г��ֵ���������Ϊ��������
    parser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, false);
    // ��������������ʱ����ֹͣ����
    parser->setFeature(XMLUni::fgXercesContinueAfterFatalError, false);

    CStringW stmp = xsdPath.c_str();
    stmp.Replace(L" ",L"%20");
    xsdPath = (const wchar_t*)stmp;

    wstring xsd =  L"http://www.iec.ch/61850/2003/SCL " + xsdPath;
    wchar_t xsdCh[400];
    // memcpyEx(xsdCh, sizeof(xsdCh), xsd);
    // ���ô�NameSpace��XSD·��
    parser->setProperty(XMLUni::fgXercesSchemaExternalSchemaLocation, (void*)xsd.c_str());
    parser->setErrorHandler((xercesc_3_1::ErrorHandler* const)&Errorhandler);
    wstring sXMLPath = AtoW(pChecker->GetFilePathToCheck());
    try
    {
        parser->parse(sXMLPath.c_str()); 
    }
    catch (const SAXParseException& toCatch) 
    { 
        int iCol = toCatch.getColumnNumber();
        int iRow = toCatch.getLineNumber();
        wstring sMes(toCatch.getMessage());
        string xml = pChecker->GetRowContentByRowNumber(iRow);
        Report(iRow, xml, WtoA(sMes).c_str());
    }
    return true;
}
