#include "LntypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>


using namespace std;
using namespace com_ximpleware;

#define KEY "5BA640"

CLntypeCheck::CLntypeCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetLNodeTypeCacheOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetLNodeTypeCacheOfICD");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfICD");
    }
}

/*

标准LNodeType检查
1、检查标准LNodeType的标准DO顺序是否正确
2、检查标准LNodeType的标准DO的数据类型是否正确
3、标准LNodeType是否存在扩展的DO类型

*/
void CLntypeCheck::CheckDO(LNodeTypeCache& inst, LNodeTypeCache& temp)
{
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DOTypeMap& mapDOType = pCacheMgr->GetDOTypeCacheOfICD();

    DOMap& instDOMap = inst.mapDO;
    DOMap& tempDOMap = temp.mapDO;

    Order& tempDoOrder = temp.doOrder;
    Order& instDoOrder = inst.doOrder;

    DOMap::iterator itTempDo = tempDOMap.begin();
    DOMap::iterator itInstDo = instDOMap.begin();

    //实例DO出现在模板DO中的位置
    Order::iterator itOrderPre = tempDoOrder.begin();
    Order::iterator itOrderCur = tempDoOrder.begin();
    
    // 标示 DO是否依序
    bool bOrdered = true;

    for (unsigned int iDo = 0; iDo < instDoOrder.size(); iDo++)
    {
        wstring sInstDoName = instDoOrder[iDo];

        DOCache& instDO = instDOMap[sInstDoName];
        DOCache& tempDO = tempDOMap[sInstDoName];

        // 得到该DO在标准模板中的位置
        itOrderCur = find(tempDoOrder.begin(), tempDoOrder.end(), sInstDoName);

        if (itOrderCur == tempDoOrder.end()) 
        {
            // 3、 扩展DO
            Report(
                pChecker->CalcRowNumberFromOffset(inst.offset),
                pChecker->GetRowContentByOffset(inst.offset),
                "LNodeType:%s 存在扩展的DO:%s",
                WtoA(inst.id).c_str(),
                WtoA(instDO.name).c_str()
                );

            continue;
        }

        if (bOrdered && itOrderCur < itOrderPre)
        {
            // 1、DO顺序与模板中不一致
            bOrdered = false;

            Report(
                pChecker->CalcRowNumberFromOffset(instDO.offset),
                pChecker->GetRowContentByOffset(instDO.offset),
                "%s",
                "DO的展示顺序应与396标准中的顺序一致"
                );
        }
        itOrderPre = itOrderCur;

        wstring sCDC = mapDOType[instDO.type].cdc;

        if (sCDC != tempDO.type)
        {
            // 2、 DO 类型不一致
            Report(
                pChecker->CalcRowNumberFromOffset(instDO.offset),
                pChecker->GetRowContentByOffset(instDO.offset),
                "%s",
                "DO 类型与标准模板不一致"
                );
        }
    }
}

bool CLntypeCheck::CheckInternal()
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
    // 得到 标准的LNodeType 和 待检测ICD文件中的LNodeType
    LNodeTypeMap& instLNodeType = pCacheMgr->GetLNodeTypeCacheOfICD();
    LNodeTypeMap& tempLNodeType = pCacheMgr->GetLNodeTypeCacheOfStandard();

    LNodeTypeMap::iterator itInst = instLNodeType.begin();
    LNodeTypeMap::iterator itTemp;

    while (itInst != instLNodeType.end())
    {
        wstring sId = itInst->second.lnClass;

        itTemp = tempLNodeType.find(sId);
        // 忽略 非标准LNodeType
        if (itTemp == tempLNodeType.end()) 
        {
            itInst++;
            continue;
        }

        // 检测 DO 顺序和类型
        CheckDO(itInst->second, itTemp->second);

        itInst++;
    }

    return true;
}