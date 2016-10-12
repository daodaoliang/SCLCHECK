#include "DotypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "3E1AE0"

CDotypeCheck::CDotypeCheck()
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
对于引用了Q/GDW-1396标准中定义的数据对象类型（DOType）应与标准中定义的内容一致。
1、检查DOType的标准DA元素、SDO子元素顺序及其所有属性是否一致 ;
2、检查DOType的数据类型是否正确
*/
void CDotypeCheck::CheckDOType(DOTypeCache& instDOType)
{
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DOTypeMap& tempDOTypeMap = pCacheMgr->GetDOTypeCacheOfStandard();

    DOTypeCache& tempDOType = tempDOTypeMap[instDOType.id];
    //忽略掉 扩展DO
    if (tempDOType.id != instDOType.id)
    {
        return;
    }

    DAMap& instDaMap = instDOType.mapDaType;
    DAMap& tempDaMap = tempDOType.mapDaType;

    DOMap& instSdoMap = instDOType.mapSDO;
    DOMap& tempSdoMap = tempDOType.mapSDO;

    // 比较模板、实例DO的da和sdo顺序
    Order& instDaOrder = instDOType.daOrder;
    Order& instSdoOrder = instDOType.sdoOrder;

    Order& tempDaOrder = tempDOType.daOrder;
    Order& tempSdoOrder = tempDOType.sdoOrder;

    Order::iterator itTempDa = tempDaOrder.begin();
    Order::iterator itTempSdo = tempSdoOrder.begin();

    // 遍历 DA 检测其顺序与所有属性 是否与标准一致
    bool bOrdered = true;

    for(unsigned int iDA = 0; iDA < instDaOrder.size(); iDA++)
    {
        wstring sDaName = instDaOrder[iDA];
        // 当前DA在标准中的位置
        Order::iterator itTempDaCur = find(tempDaOrder.begin(), tempDaOrder.end(), sDaName);
        
        if (bOrdered && itTempDaCur < itTempDa)
        {
            // 顺序不匹配
            Report(
                pChecker->CalcRowNumberFromOffset(instDOType.offset),
                pChecker->GetRowContentByOffset(instDOType.offset),
                "【DO:%s】下属DA的顺序与396标准中的顺序一致",
                WtoA(instDOType.id).c_str()
                );

            bOrdered = false;
        }
        itTempDa = itTempDaCur;

        DACache& instDA = instDaMap[sDaName];
        DACache& tempDA = tempDaMap[sDaName];
        if (!(instDA.name == tempDA.name
            && instDA.bType == tempDA.bType
            && instDA.type == tempDA.type
            && instDA.dchg == tempDA.dchg
            && instDA.qchg == tempDA.qchg))
        {
            //所有属性并非全部一致
            Report(
                pChecker->CalcRowNumberFromOffset(instDA.offset),
                pChecker->GetRowContentByOffset(instDA.offset),
                "【DO:%s】下【DA:%s】的所有属性与标准不完全一致",
                WtoA(instDOType.id).c_str(),
                WtoA(instDA.name).c_str()
                );
        }
    }

    // 遍历 SDO 检测其顺序与所有属性 是否与标准一致
    bOrdered = true;

    for(unsigned int iSdo = 0; iSdo < instSdoOrder.size(); iSdo++)
    {
        wstring sSdoName = instSdoOrder[iSdo];
        // 当前sdo在标准中的位置
        Order::iterator itTempSdoCur = find(tempSdoOrder.begin(), tempSdoOrder.end(), sSdoName);

        if (bOrdered && itTempSdoCur < itTempSdo)
        {
            // 顺序不匹配
            Report(
                pChecker->CalcRowNumberFromOffset(instDOType.offset),
                pChecker->GetRowContentByOffset(instDOType.offset),
                "【DO:%s】下属SDO的顺序与396标准中的顺序一致",
                WtoA(instDOType.id).c_str()
                );

            bOrdered = false;
        }
        itTempSdo = itTempSdoCur;

        DOCache& instSdo = instSdoMap[sSdoName];
        DOCache& tempSdo = tempSdoMap[sSdoName];
        if (!(instSdo.name == tempSdo.name && instSdo.type == tempSdo.type))
        {
            //所有属性并非全部一致
            Report(
                pChecker->CalcRowNumberFromOffset(instSdo.offset),
                pChecker->GetRowContentByOffset(instSdo.offset),
                "【DO:%s】下【SDO:%s】的所有属性与标准不完全一致",
                WtoA(instDOType.id).c_str(),
                WtoA(instSdo.name).c_str()
                );
        }
    }

    // 检测数据类型是否一致
    if(instDOType.cdc != tempDOType.cdc)
    {
        Report(
            pChecker->CalcRowNumberFromOffset(instDOType.offset),
            pChecker->GetRowContentByOffset(instDOType.offset),
            "【DO:%s】的数据类型与标准不一致",
            WtoA(instDOType.id).c_str()
            );
    }
}

bool CDotypeCheck::CheckInternal()
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
