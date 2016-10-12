#include "ConfReportCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "0CB373"

CConfReportCheck::CConfReportCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_Services");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
    }
}

bool CConfReportCheck::CheckInternal()
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

void CConfReportCheck::CheckReportCB_InCacheMgr(ICDCache& icd)
{
    CConfReportCheck* pConfReportCheck = CConfReportCheck::GetInstance();

    if (!pConfReportCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();

    int maxReport = icd.services.report_max;
    if (maxReport == -1)
    {
        pConfReportCheck->Report(
            pChecker->CalcRowNumberFromOffset(icd.services.offset),
            pChecker->GetRowContentByOffset(icd.services.offset),
            "ConfReportControl的max属性未配置");
        return;
    }

    LDeviceCacheMap& devices = icd.mapLDevice;
    LDeviceCacheMap::iterator itLD = devices.begin();
    int nReport = 0;
    while (itLD != devices.end())
    {
        LDeviceCache& device = itLD->second;
        nReport += device.nReportCB;
        itLD++;
    }

    if (nReport > maxReport)
    {
        pConfReportCheck->Report(
            pChecker->CalcRowNumberFromOffset(icd.services.report_offset),
            pChecker->GetRowContentByOffset(icd.services.report_offset),
            "报告控制块ReportControl的个数大于ConfReportControl的max属性值：ReportControl个数= %d  ConfReportControl(max)= %d ",
            nReport,
            maxReport);
    }
}