#include "ServerCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "9A2FE1"

CServerCheck::CServerCheck()
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
    }
}

bool CServerCheck::CheckInternal()
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

/*
        每个LDevice对象至少应包含LLN0及LPHD两个逻辑节点
*/
void CServerCheck::CheckLLN0LPHD_InCacheMgr(LDeviceCache& lDevice)
{
    CServerCheck* pServerCheck = CServerCheck::GetInstance();

    if (!pServerCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();

    if (!lDevice.bLLN0 || !lDevice.bLPHD)
    {
        pServerCheck->Report(
            pChecker->CalcRowNumberFromOffset(lDevice.offset),
            pChecker->GetRowContentByOffset(lDevice.offset),
            "LDevice对象至少应包含LLN0及LPHD两个逻辑节点"
            );
    }
}
/*
        Server对象中至少应包含一个LDevice对象
*/
void CServerCheck::CheckhasLDevice_InCacheMgr(bool bHasLDevice, int offset)
{
    CServerCheck* pServerCheck = CServerCheck::GetInstance();

    if (!pServerCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();

    if (bHasLDevice == false)
    {
        pServerCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "Server对象中至少应包含一个LDevice对象"
            );
    }
}