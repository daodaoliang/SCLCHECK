#include "CdcCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

#include "../../ScdCheckCommon/ValidateCache.h"

using namespace std;
using namespace com_ximpleware;

#define KEY "1F61CB"

CCdcCheck::CCdcCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfICD");
    }
}

bool CCdcCheck::CheckInternal()
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
    // 遍历所有的DOType,检测其CDC属性
    DOTypeMap& instDOTypeMap = pCacheMgr->GetDOTypeCacheOfICD();

    DOTypeMap::iterator itInst = instDOTypeMap.begin();

    wchar_t* arrCdc[] = {L"DPC", L"INC", L"BSC", L"SPG",
                         L"ING", L"ASG", L"STG", L"DPL",
                         L"LPL", L"ACT", L"ACD", L"MV", L"CMV",
                         L"WYE", L"DEL", L"SPC", L"SAV", L"SPS", L"INS"};

    vector<wstring> vecCdc(arrCdc, arrCdc+sizeof(arrCdc)/sizeof(wchar_t*));

    while (itInst != instDOTypeMap.end())
    {
        wstring sCdc = itInst->second.cdc;

        if (vecCdc.end() == find(vecCdc.begin(), vecCdc.end(), sCdc))
        {
            // 自定义CDC
            Report(
                pChecker->CalcRowNumberFromOffset(itInst->second.offset),
                pChecker->GetRowContentByOffset(itInst->second.offset),
                "【DOType:%s】中存在自扩充【CDC:%s】,不在标准【DPC,INC,BSC,SPG,ING,ASG,STG,DPL,LPL】中",
                WtoA(itInst->second.id).c_str(),
                WtoA(sCdc).c_str());
        }
        itInst++;
    }

    return true;
}
