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

    // 取本条目的配置信息并配置到本对象
    if (CConfigMgr::GetInstance()->GetItemConfig(KEY, cfg))
    {
        SetConfig(cfg);
    }

    // 如果启用了本条目，则向检测器注册
    if (IsEnabled())
    {
        CScdChecker::GetInstance()->RegisterCheckItem(this);

        // 在此处添加需要某某类型的缓存信息，还可以继续添加条目
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
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    // 输出管理器
    COutputMgr *pOutputMgr = COutputMgr::GetInstance();
    // 配置文件管理器
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
    // 开启校验功能
    parser->setFeature(XMLUni::fgXercesSchema, true);
    // 将校验中出现的问题设置为致命错误
    parser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, false);
    // 当遇到致命错误时，不停止解析
    parser->setFeature(XMLUni::fgXercesContinueAfterFatalError, false);

    CStringW stmp = xsdPath.c_str();
    stmp.Replace(L" ",L"%20");
    xsdPath = (const wchar_t*)stmp;

    wstring xsd =  L"http://www.iec.ch/61850/2003/SCL " + xsdPath;
    wchar_t xsdCh[400];
    // memcpyEx(xsdCh, sizeof(xsdCh), xsd);
    // 设置带NameSpace的XSD路径
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
