#include "RedLntypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

#include <tchar.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "2C98FA"

CRedLntypeCheck::CRedLntypeCheck()
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

/*
检查是否存在扩展的LNodeType，
1、扩展的LNodeType应符合命名规范（制造厂商前缀_装置型号_版本号_LN类名(_其它后缀)）
2、扩展LNodeType的命名空间要存在
*/
void CRedLntypeCheck::CheckExtendLNodeType(LNodeTypeCache& instLNodeType)
{
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();

    wstring sLNodeTypeName = instLNodeType.id;

    vector<wstring> vecNames = split_string(sLNodeTypeName, L"_");

    if (vecNames.size() < 4)
    {
        // 1、扩展的LNodeType应符合命名规范（制造厂商前缀_装置型号_版本号_LN类名(_其它后缀)）
        Report(
            pChecker->CalcRowNumberFromOffset(instLNodeType.offset),
            pChecker->GetRowContentByOffset(instLNodeType.offset),
            "%s",
            "扩展的LNodeType应符合命名规范（制造厂商前缀_装置型号_版本号_LN类名(_其它后缀)）"
            );
    }
    // 2、扩展LNodeType的命名空间要存在

}

bool CRedLntypeCheck::CheckInternal()
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
        // 扩展 LNodeType
        if (itTemp == tempLNodeType.end()) 
        {
            CheckExtendLNodeType(itInst->second);
        }
        itInst++;
    }

    return true;
}
