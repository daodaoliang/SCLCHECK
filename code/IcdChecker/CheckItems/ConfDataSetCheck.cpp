#include "ConfDataSetCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "AF455E"

CConfDataSetCheck::CConfDataSetCheck()
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
    }
}

bool CConfDataSetCheck::CheckInternal()
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

void CConfDataSetCheck::CheckDataset_InCacheMgr(ICDCache& icd)
{
    CConfDataSetCheck* pConfDataSetCheck = CConfDataSetCheck::GetInstance();

    if (!pConfDataSetCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();

    int maxDataset = icd.services.dataset_max;

    if (maxDataset == -1)
    {
        pConfDataSetCheck->Report(
            pChecker->CalcRowNumberFromOffset(icd.services.offset),
            pChecker->GetRowContentByOffset(icd.services.offset),
            "ConfDataset的max属性未配置",
            maxDataset);
        return;
    }

    LDeviceCacheMap& devices = icd.mapLDevice;
    LDeviceCacheMap::iterator itLD = devices.begin();

    int nDataset = 0;
    while (itLD != devices.end())
    {
        LDeviceCache& device = itLD->second;
        nDataset += device.mapDataset.size();
        itLD++;
    }

    if (nDataset > maxDataset)
    {
        pConfDataSetCheck->Report(
            pChecker->CalcRowNumberFromOffset(icd.services.dataset_offset),
            pChecker->GetRowContentByOffset(icd.services.dataset_offset),
            "数据集DataSet的个数大于ConfDataset的max属性值【%d】",
            maxDataset);
    }
}