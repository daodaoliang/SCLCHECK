#include "GsSmvCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "467747"

CGsSmvCheck::CGsSmvCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
    }
}

bool CGsSmvCheck::CheckInternal()
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
// IED中应包含GSEControl和SampledValueControl
void CGsSmvCheck::CheckGseSmv_InCacheMgr(ICDCache& icd)
{
    CGsSmvCheck* pGsSmvCheck = CGsSmvCheck::GetInstance();

    if (!pGsSmvCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();

    LDeviceCacheMap& mapLd = icd.mapLDevice;
    LDeviceCacheMap::iterator itLd = mapLd.begin();

    while (itLd != mapLd.end())
    {
        LDeviceCache& device = itLd->second;

        if(device.bHasGoose || device.bHasSmv)
        {
            return;
        }
        itLd++;
    }

    pGsSmvCheck->Report(
        pChecker->CalcRowNumberFromOffset(0),
        pChecker->GetRowContentByOffset(0),
        "IED中不存在GSEControl和SampledValueCotrol,应至少包含其中一个");
}