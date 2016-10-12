#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>
#include "../../ScdCrcCalculator-UnFixedAttribute/ScdCrcCalculator.h"

#include "CrcBlankValidCheck.h"
using namespace std;
using namespace com_ximpleware;

#define KEY "DF89PO"

#ifdef _WIN64
#pragma comment(lib, "../lib/x64/ScdCrcCalculator264.lib")
#else
#pragma comment(lib, "../lib/win32/ScdCrcCalculator2.lib")
#endif

CCrcBlankValidCheck::CCrcBlankValidCheck()
{
    CheckItemConfig cfg;
    hWorkThread = NULL;
    lzText = NULL;
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
        hWorkThread = CreateThread(NULL, 0, ScdCRCWorkProc, (LPVOID)(this), 0, NULL);
    }
}

CCrcBlankValidCheck::~CCrcBlankValidCheck()
{
    if (NULL != hWorkThread)
    {
        WaitForSingleObject(hWorkThread, 1000);
        TerminateThread(hWorkThread, 0);
        CloseHandle(hWorkThread);
        hWorkThread = NULL;
    }
    if (NULL != lzText)
    {
        CalcCrcFreeMemory(lzText);
    }
}

DWORD CALLBACK CCrcBlankValidCheck::ScdCRCWorkProc(LPVOID lpParam)
{
    SafeDebugMessageA("启动CRC线程");
    CCrcBlankValidCheck *pCRC = (CCrcBlankValidCheck *)lpParam;
    string path = CScdChecker::GetInstance()->GetFilePathToCheck();
    char szErrorMessageUtf8[1024];
    pCRC->lzText = CalcCrc(path.c_str(), szErrorMessageUtf8, sizeof(szErrorMessageUtf8));

    if (NULL != pCRC->lzText)
    {
        SafeDebugMessageA("CRC线程 成功返回");
        return 1;
    }else
    {
        SafeDebugMessageA("CRC线程 返回NULL");
        return 0;
    }

}

bool CCrcBlankValidCheck::CheckInternal()
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
    if (NULL == hWorkThread)
    {
        return false;
    }
    SafeDebugMessageA("等待CRC线程");
    WaitForSingleObject(hWorkThread, INFINITE);
    DWORD lpExitCode = 0;
    if (GetExitCodeThread(hWorkThread, &lpExitCode) && lpExitCode==STILL_ACTIVE)
    {
        COutputMgr::GetInstance()->Error(
            "检测项目%s,%s时发生错误%s\r\n",
            CCrcBlankValidCheck::GetInstance()->GetItemConfig().strCheckItemKey.c_str(),
            CCrcBlankValidCheck::GetInstance()->GetItemConfig().strCheckItemName.c_str(),
            "线程结束异常");
        return true;
    }
    CloseHandle(hWorkThread);
    hWorkThread = NULL;
    SafeDebugMessageA("CRC线程退出");
    if ((lzText != NULL) && lpExitCode && !CreatMap())
    {
        COutputMgr::GetInstance()->Error(
            "检测项目%s,%s时发生错误%s\r\n",
            CCrcBlankValidCheck::GetInstance()->GetItemConfig().strCheckItemKey.c_str(),
            CCrcBlankValidCheck::GetInstance()->GetItemConfig().strCheckItemName.c_str(),
            "创建Map异常");
        return true;
    }
    SCDCache *p_SCDCache = &pCacheMgr->m_SCDCache;
    for (IEDMap::iterator itIED = p_SCDCache->mapIED.begin(); itIED != p_SCDCache->mapIED.end(); itIED++)
    {
        IEDCache *iedCache = &itIED->second;
        std::map<wstring, wstring>::iterator itCRC = m_CrcMap.find(iedCache->name);
        if (itCRC != m_CrcMap.end())
        {
            if (iedCache->CRC != itCRC->second)
            {
                int row = pChecker->CalcRowNumberFromOffset(iedCache->offset);
                string xml = pChecker->GetRowContentByOffset(iedCache->offset);
                string msg = "SCD文件中IED过程层虚端子CRC校验码不正确:IED name=%s SCD文件中CRC=%s 标准计算CRC=%s";
                Report(row, xml, msg.c_str(), WtoA(iedCache->name).c_str(), WtoA(iedCache->CRC).c_str(), WtoA(itCRC->second).c_str());
            }
        }
    }
    SafeDebugMessageA("结束CRC检测");
    return true;
}

bool CCrcBlankValidCheck::CreatMap()
{
    try
    {
        VTDGen vg;
        //char *da = new char(strlen(lzText));
        //strcpy(da, lzText);
        //vg.setDoc(da, strlen(lzText));
        vg.setDoc(const_cast<char*>(lzText), strlen(lzText));
        try{
            vg.parse(false);
        }
        catch(...)
        {
            return false;
        }

        VTDNav *m_vn = vg.getNav();

        m_vn->toElement(ROOT);
        if (m_vn->toElement(FIRST_CHILD, L"IED"))
        {
            do 
            {
                wstring iedName, crc;
                if (m_vn->toElement(FIRST_CHILD, L"NAME"))
                {
                    iedName = VtdGetTextValue(m_vn);
                    m_vn->toElement(PARENT);
                }
                if (m_vn->toElement(FIRST_CHILD, L"CRC"))
                {
                    crc = VtdGetTextValue(m_vn);
                    m_vn->toElement(PARENT);
                }
                m_CrcMap.insert(make_pair(iedName, crc));
            } while (m_vn->toElement(NEXT_SIBLING, L"IED"));
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

