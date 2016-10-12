#include "RedDatypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "165A0B"

CRedDatypeCheck::CRedDatypeCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDATypeCacheOfICD");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDATypeCacheOfStandard");
    }
}

bool CRedDatypeCheck::CheckInternal()
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

    // 得到 标准文件 和 ICD检测文件中的DAType
    DATypeMap& instDAMap = pCacheMgr->GetDATypeCacheOfICD();
    DATypeMap& tempDAMap = pCacheMgr->GetDATypeCacheOfStandard();

    DATypeMap::iterator itDA = instDAMap.begin();
    /*
        检查是否存在扩展DA，且扩展DA应符合命名规范
        （制造厂商前缀_装置型号_版本号_DA类名(_其它后缀)）
    */
    while (itDA != instDAMap.end())
    {
        DATypeCache& da = tempDAMap[itDA->first];
        if (da.id.empty())
        {
            // 扩展 DA
            wstring daName = da.id;
            vector<wstring>& vecSub = split_string(daName, L"_");
            if(vecSub.size() < 4)
            {
                Report(
                    pChecker->CalcRowNumberFromOffset(da.offset),
                    pChecker->GetRowContentByOffset(da.offset),
                    "%s",
                    "扩展DA应符合命名规范【制造厂商前缀_装置型号_版本号_DA类名(_其它后缀)】");
            }
        }
        itDA++;
    }

    return true;
}
