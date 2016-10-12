#include "DoiValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "28F210"

CDoiValidCheck::CDoiValidCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DOI");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_SDI");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DAI");
    }
}

bool CDoiValidCheck::CheckInternal()
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

void CDoiValidCheck::CheckDescAnddU_InCacheMgr(LNCache& ln, DOICache& doi, wstring sdU, int offset)
{
    CDoiValidCheck* pDoiValidCheck = CDoiValidCheck::GetInstance();

    if (!pDoiValidCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    if (doi.sdesc.empty() || sdU.empty() || doi.sdesc != sdU)
    {
        pDoiValidCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "实例化DOI元素的“desc”属性值与“dU”属性值不一致:LDevice inst=%s LN=%s DOI name=%s desc=\"%s\" dU=\"%s\" ",
            WtoA(ln.sLdInst).c_str(),
            WtoA(ln.sLnClass+ln.sInst).c_str(),
            WtoA(doi.sname).c_str(),
            WtoA(doi.sdesc).c_str(),
            WtoA(sdU).c_str());
    }
}