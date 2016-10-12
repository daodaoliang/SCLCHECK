#include "MLntypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "20544B"

CMLntypeCheck::CMLntypeCheck()
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

    }
}
// 检查标准LNodeType的强制（或条件强制）DO是否存在
void CMLntypeCheck::CheckMLNodeTypeDo(LNodeTypeCache& inst, LNodeTypeCache& temp)
{
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();

    DOMap& instDOMap = inst.mapDO;
    DOMap& tempDOMap = temp.mapDO;

    Order& tempDoOrder = temp.doOrder;
    Order& instDoOrder = inst.doOrder;

    DOMap::iterator itTempDo = tempDOMap.begin();
    DOMap::iterator itInstDo = instDOMap.begin();
   
    // 遍历标准模板数据中的强制DO,查找实例数据中是否存在
    while (itTempDo != tempDOMap.end())
    {
        DOCache& objDo = itTempDo->second;

        if (objDo.eOption != L"M")
        {
            itTempDo++;
            continue;
        }

        if (instDOMap.end() == instDOMap.find(objDo.name))
        {
            // 强制DO不存在
            Report(
                pChecker->CalcRowNumberFromOffset(inst.offset),
                pChecker->GetRowContentByOffset(inst.offset),
                "LNodeType中强制DO:%s 不存在",
                WtoA(objDo.name).c_str()
                );
        }
        itTempDo++;
    }
}

bool CMLntypeCheck::CheckInternal()
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
    LNodeTypeMap& instLNodeType = pCacheMgr->GetLNodeTypeCacheOfICD();
    LNodeTypeMap& tempLNodeType = pCacheMgr->GetLNodeTypeCacheOfStandard();

    LNodeTypeMap::iterator itInst = instLNodeType.begin();
    LNodeTypeMap::iterator itTemp;

    while (itInst != instLNodeType.end())
    {
        wstring sId = itInst->second.lnClass;

        itTemp = tempLNodeType.find(sId);
        // 忽略 扩展 LNodeType
        if (itTemp == tempLNodeType.end()) 
        {
            itInst++;
            continue;
        }
        // 检查标准LNodeType的强制（或条件强制）DO是否存在
        CheckMLNodeTypeDo(itInst->second, itTemp->second);
        itInst++;
    }

    return true;
}
