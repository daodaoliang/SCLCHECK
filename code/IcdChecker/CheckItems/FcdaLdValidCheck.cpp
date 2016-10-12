#include "FcdaLdValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "58E4DA"

CFcdaLdValidCheck::CFcdaLdValidCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_IED");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DataSet");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_FCDA");
    }
}

bool CFcdaLdValidCheck::CheckInternal()
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

    return true;
}

void CFcdaLdValidCheck::CheckCrossLDevice(FcdaCache& fcda, wstring sLd)
{
    CFcdaLdValidCheck* pFcdaLdValidCheck = CFcdaLdValidCheck::GetInstance();

    if (!pFcdaLdValidCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    if (fcda.sLdInst != sLd)
    {
        wstring sKey = fcda.sLdInst + L"/" + fcda.sPrefix + fcda.sLnClass + fcda.sLnInst + L"." + fcda.sDoName;
        if (!fcda.sDaName.empty())
        {
            sKey += L".";
            sKey += fcda.sDaName;
        }

        pFcdaLdValidCheck->Report(
            pChecker->CalcRowNumberFromOffset(fcda.offset),
            pChecker->GetRowContentByOffset(fcda.offset),
            "数据集中包含的FCDA不在LDevice中：LDevice inst = %s  FCDA=%s",
            WtoA(fcda.sLdInst).c_str(),
            WtoA(sKey).c_str());
    }
}