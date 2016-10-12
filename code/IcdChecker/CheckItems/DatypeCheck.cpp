#include "DatypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "DE1D56"

CDatypeCheck::CDatypeCheck()
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
/*
    对于引用了Q/GDW-1396标准中定义的数据属性类型（DAType），
    应与标准中定义的内容一致(检查DAType的BDA元素顺序是否一致，
    检查DAType的数据类型是否正确)
    1、BDA元素顺序与附件中一致；
    2、DAType下每个BDA的name、bType、type完全一致；
    3、DAType下的BDA与附件中相比，无多余的BDA,如果有多余的BDA，需要提示是扩展的BDA;
*/
void CDatypeCheck::CheckDAType(DATypeCache& instDA, DATypeCache& tempDA)
{
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();

    BDAMap& instBDA = instDA.mapBDA;
    BDAMap& tempBDA = tempDA.mapBDA;

    Order& orderBDAInst = instDA.bdaOrder;
    Order& orderBDATemp = tempDA.bdaOrder;

    Order::iterator itOrderBDAInst = orderBDAInst.begin();
    // 实例BDA在标准BDA中出现的位置
    Order::iterator itOrderBDATemp = orderBDATemp.begin();

    bool bOrdered = true;
    while (itOrderBDAInst != orderBDAInst.end())
    {
        // 顺序校验
        Order::iterator itOrder = find(orderBDATemp.begin(), orderBDATemp.end(), *itOrderBDAInst);
        if (itOrder == orderBDATemp.end())
        {
            // 扩展BDA
            Report(
                pChecker->CalcRowNumberFromOffset(instDA.offset),
                pChecker->GetRowContentByOffset(instDA.offset),
                "【DA:%s】中存在扩展【BDA:%s】",
                WtoA(instDA.id).c_str(),
                WtoA(*itOrderBDAInst).c_str());

            itOrderBDAInst++;
            continue;
        }
        if (bOrdered && (itOrder < itOrderBDATemp))
        {
            // 顺序颠倒
            bOrdered = false;

            Report(
                pChecker->CalcRowNumberFromOffset(instDA.offset),
                pChecker->GetRowContentByOffset(instDA.offset),
                "【DA:%s】中BDA的顺序与标准不一致",
                WtoA(instDA.id).c_str());
        }
        itOrderBDATemp = itOrder;

        // 属性比较
        BDACache& inst = instBDA[*itOrderBDAInst];
        BDACache& temp = tempBDA[inst.name];

        if((inst.type != temp.type 
            || inst.bType != temp.bType))
        {
            // 属性不一致
            Report(
                pChecker->CalcRowNumberFromOffset(inst.offset),
                pChecker->GetRowContentByOffset(inst.offset),
                "【BDA:%s】属性值与标准不一致",
                WtoA(inst.name).c_str());
        }
        itOrderBDAInst++;
    }
}

bool CDatypeCheck::CheckInternal()
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

    while (itDA != instDAMap.end())
    {        
        CheckDAType(itDA->second,tempDAMap[itDA->first]);

        itDA++;
    }
    return true;
}
