#include "ReportCtrlCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "8E9CE7"

CReportCtrlCheck::CReportCtrlCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetReportControlTypeOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_IED");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
    }
}

bool CReportCtrlCheck::CheckInternal()
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

void CReportCtrlCheck::CheckName_InCacheMgr(wstring sName, int offset)
{
    CReportCtrlCheck* pReportCtrlCheck = CReportCtrlCheck::GetInstance();

    if (!pReportCtrlCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    ReportControlTypeMap& mapReport = pCacheMgr->GetReportControlTypeOfStandard();

    ReportControlTypeMap::iterator itReport = mapReport.begin();

    while (itReport != mapReport.end())
    {
        wstring sNameTemp = itReport->second.sName;
        int iPos = sName.find(sNameTemp);
        if (iPos == 0)
        {
            return;
        }
        itReport++;
    }

    pReportCtrlCheck->Report(
        pChecker->CalcRowNumberFromOffset(offset),
        pChecker->GetRowContentByOffset(offset),
        "报告控制块的命名不符合Q/GDW-1396标准(前缀+数字): ReportControl name=%s",
        WtoA(sName).c_str()
        );
}