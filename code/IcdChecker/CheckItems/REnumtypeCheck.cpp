#include "REnumtypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "56AB89"

CREnumtypeCheck::CREnumtypeCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetEnumTypeCacheOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetEnumTypeCacheOfICD");
    }
}

bool CREnumtypeCheck::CheckInternal()
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
    // 得到标准和ICD中的EnumType
    EnumTypeMap& tempEnumMap = pCacheMgr->GetEnumTypeCacheOfStandard();
    EnumTypeMap& instEnumMap = pCacheMgr->GetEnumTypeCacheOfICD();

    // 检查是否存在扩展的EnumType
    EnumTypeMap::iterator itInstEnum = instEnumMap.begin();
    EnumTypeMap::iterator itTempEnum = tempEnumMap.begin();

    while (itInstEnum != instEnumMap.end())
    {
        itTempEnum = tempEnumMap.find(itInstEnum->first);
        if (itTempEnum == tempEnumMap.end())
        {
            // 扩展EnumType
            Report(
                pChecker->CalcRowNumberFromOffset(itInstEnum->second.offset),
                pChecker->GetRowContentByOffset(itInstEnum->second.offset),
                "存在扩展的EnumType【%s】",
                WtoA(itInstEnum->first).c_str());
        }
        itInstEnum++;
    }

    return true;
}
