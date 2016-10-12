#include "EnumtypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "145768"

CEnumtypeCheck::CEnumtypeCheck()
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

bool CEnumtypeCheck::CheckInternal()
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

    // 检查标准EnumType的类型和值是否正确
    EnumTypeMap::iterator itInstEnum = instEnumMap.begin();
    EnumTypeMap::iterator itTempEnum = tempEnumMap.begin();

    while (itInstEnum != instEnumMap.end()
        && (itTempEnum = tempEnumMap.find(itInstEnum->first)) != tempEnumMap.end())
    {
        vector<wstring>& instVal = itInstEnum->second.vecVal;
        vector<wstring>& tempVal = itTempEnum->second.vecVal;
        // 比较该枚举类型的值
        if(instVal.size() != tempVal.size())
        {
            Report(
                pChecker->CalcRowNumberFromOffset(itInstEnum->second.offset),
                pChecker->GetRowContentByOffset(itInstEnum->second.offset),
                "枚举类型【%s】的值数目与标准不一致",
                WtoA(itInstEnum->first).c_str());

            itInstEnum++;
            itTempEnum++;

            continue;
        }

        for (unsigned int iVal = 0; iVal < instVal.size(); iVal++)
        {
            if (instVal[iVal] != tempVal[iVal])
            {
                //值不一致
                Report(
                    pChecker->CalcRowNumberFromOffset(itInstEnum->second.offset),
                    pChecker->GetRowContentByOffset(itInstEnum->second.offset),
                    "枚举类型【%s】的值【%s】与标准的值【%s】不一致",
                    WtoA(itInstEnum->first).c_str(),
                    WtoA(instVal[iVal]).c_str(),
                    WtoA(tempVal[iVal]).c_str());
            }
        }

        itInstEnum++;
        itTempEnum++;
    }

    return true;
}
