#include "MDatypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "B73E6D"

CMDatypeCheck::CMDatypeCheck()
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

bool CMDatypeCheck::CheckInternal()
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
    /*
        数据类型模板检查标准DAType的强制BDA元素是否存在
    */
    // 得到 标准文件 和 ICD检测文件中的DAType
    DATypeMap& instDAMap = pCacheMgr->GetDATypeCacheOfICD();
    DATypeMap& tempDAMap = pCacheMgr->GetDATypeCacheOfStandard();

    DATypeMap::iterator itDA = instDAMap.begin();
    // 遍历DA
    while (itDA != instDAMap.end())
    {        
        DATypeCache& instDA = itDA->second;
        DATypeCache& tempDA = tempDAMap[itDA->first];
        if (tempDA.id.empty())
        {
            itDA++;
            continue;
        }
        // 遍历BDA
        BDAMap& instBDA = instDA.mapBDA;
        BDAMap& tempBDA = tempDA.mapBDA;
        BDAMap::iterator itBDA = tempBDA.begin();
        while (itBDA != tempBDA.end())
        {
            if(itBDA->second.eOption == L"M"
                && instBDA.find(itBDA->first) == instBDA.end())
            {
                // 强制BDA不存在
                Report(
                    pChecker->CalcRowNumberFromOffset(instDA.offset),
                    pChecker->GetRowContentByOffset(instDA.offset),
                    "标准【DA:%s】的强制【BDA:%s】不存在",
                    WtoA(instDA.id).c_str(),
                    WtoA(itBDA->first).c_str());
            }
            itBDA++;
        }
        itDA++;
    }
    return true;
}
