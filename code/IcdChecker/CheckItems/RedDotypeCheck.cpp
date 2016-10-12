#include "RedDotypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "A63491"

CRedDotypeCheck::CRedDotypeCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfICD");
    }
}
/*
检查是否存在扩展的DO，扩展时应符合命名规范
（制造厂商前缀_装置型号_版本号_DO类名(_其它后缀)）
*/
void CRedDotypeCheck::CheckDOType(DOTypeCache& instDOType)
{
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DOTypeMap& tempDOTypeMap = pCacheMgr->GetDOTypeCacheOfStandard();
    
    DOTypeMap::iterator itTempDOType = tempDOTypeMap.find(instDOType.id);
    
    if (itTempDOType != tempDOTypeMap.end())
    {
        return;
    }
    // 扩展DO
    vector<wstring>& vecName = split_string(instDOType.id,L"_");
    if(vecName.size() < 4)
    {
        Report(
            pChecker->CalcRowNumberFromOffset(instDOType.offset),
            pChecker->GetRowContentByOffset(instDOType.offset),
            "%s",
            "扩展的DO应符合命名规范（制造厂商前缀_装置型号_版本号_DO类名(_其它后缀))"
            );
    }
}

bool CRedDotypeCheck::CheckInternal()
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
    // 得到ICD检测文件中的DOType
    DOTypeMap& instDOMap = pCacheMgr->GetDOTypeCacheOfICD();
    DOTypeMap::iterator itDO = instDOMap.begin();
    while(itDO != instDOMap.end())
    {
        CheckDOType(itDO->second);

        itDO++;
    }

    return true;
}
