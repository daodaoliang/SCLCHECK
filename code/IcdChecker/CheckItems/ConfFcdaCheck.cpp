#include "ConfFcdaCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "B3824A"

CConfFcdaCheck::CConfFcdaCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DataSet");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_FCDA");
    }
}

bool CConfFcdaCheck::CheckInternal()
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

void CConfFcdaCheck::CheckFcdaCount_InCacheMgr(DatasetCache& dataset, int maxFcda)
{
    CConfFcdaCheck* pConfFcdaCheck = CConfFcdaCheck::GetInstance();

    if (!pConfFcdaCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    int nFcdaCount = dataset.vecFcda.size();
    if (maxFcda == -1)
    {
        pConfFcdaCheck->Report(
            pChecker->CalcRowNumberFromOffset(dataset.offset),
            pChecker->GetRowContentByOffset(dataset.offset),
            "ConfDataSet中maxAttributes属性未配置"
            );
        return;
    }
    if (nFcdaCount > maxFcda)
    {
        pConfFcdaCheck->Report(
            pChecker->CalcRowNumberFromOffset(dataset.offset),
            pChecker->GetRowContentByOffset(dataset.offset),
            "数据集配置的FCDA实际个数【%d】大于ConfDataset的maxAttributes属性值：AccessPoint=%s LDevice=%s DataSet=%s ConfDataset(maxAttributes)=%d ",
            nFcdaCount,
            WtoA(dataset.sAccessPoint).c_str(),
            WtoA(dataset.sLDevice).c_str(),
            WtoA(dataset.sName).c_str(),
            maxFcda);
    }
    else if (nFcdaCount == 0)
    {
        pConfFcdaCheck->Report(
            pChecker->CalcRowNumberFromOffset(dataset.offset),
            pChecker->GetRowContentByOffset(dataset.offset),
            "数据集配置的FCDA个数【%d】不能为 0 ：AccessPoint=%s LDevice=%s DataSet=%s ",
            nFcdaCount,
            WtoA(dataset.sAccessPoint).c_str(),
            WtoA(dataset.sLDevice).c_str(),
            WtoA(dataset.sName).c_str());
    }
}