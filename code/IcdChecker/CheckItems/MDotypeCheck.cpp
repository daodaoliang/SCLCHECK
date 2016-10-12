#include "MDotypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "BB4024"

CMDotypeCheck::CMDotypeCheck()
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
// 检查标准DOType的强制DA元素是否存在
void CMDotypeCheck::CheckMDOTypeDA(DOTypeCache& inst, DOTypeCache& temp)
{
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();

    DAMap& instDAMap = inst.mapDaType;
    DAMap& tempDAMap = temp.mapDaType;

    DAMap::iterator itTempDA = tempDAMap.begin();
    DAMap::iterator itInstDA = instDAMap.begin();

    // 遍历标准模板数据中的强制DO,查找实例数据中是否存在
    while (itTempDA != tempDAMap.end())
    {
        DACache& objDA = itTempDA->second;

        if (objDA.eOption != L"M")
        {
            itTempDA++;
            continue;
        }

        if (instDAMap.end() == instDAMap.find(objDA.name))
        {
            // 强制DA不存在
            Report(
                pChecker->CalcRowNumberFromOffset(inst.offset),
                pChecker->GetRowContentByOffset(inst.offset),
                "【DO:%s】中强制【DA:%s】不存在",
                WtoA(inst.id).c_str(),
                WtoA(objDA.name).c_str()
                );
        }
        itTempDA++;
    }
}

bool CMDotypeCheck::CheckInternal()
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
    DOTypeMap& instDOTypeMap = pCacheMgr->GetDOTypeCacheOfICD();
    DOTypeMap& tempDOTypeMap = pCacheMgr->GetDOTypeCacheOfStandard();

    DOTypeMap::iterator itInst = instDOTypeMap.begin();
    DOTypeMap::iterator itTemp;

    while (itInst != instDOTypeMap.end())
    {
        wstring sId = itInst->second.id;

        itTemp = tempDOTypeMap.find(sId);
        // 忽略 扩展 DOType
        if (itTemp == tempDOTypeMap.end()) 
        {
            itInst++;
            continue;
        }
        // 检查标准DOType的强制（或条件强制）DA是否存在
        CheckMDOTypeDA(itInst->second, itTemp->second);
        itInst++;
    }


    return true;
}
