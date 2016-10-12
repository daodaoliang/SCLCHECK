#include "LDdoiCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "E9DFBC"

CLDdoiCheck::CLDdoiCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetLNodeTypeCacheOfICD");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfICD");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDATypeCacheOfICD");

        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_IED");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DOI");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_SDI");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DAI");
    }
}

bool CLDdoiCheck::CheckInternal()
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

    return true;
}

bool CLDdoiCheck::CheckLnTypeValid_InCacheMgr(LNCache& ln, DOMap& mapDo)
{
    CLDdoiCheck* pLDdoiCheck = CLDdoiCheck::GetInstance();

    if (!pLDdoiCheck->IsEnabled())
    {
        return false;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    LNodeTypeMap& mapLNodeType = pCacheMgr->GetLNodeTypeCacheOfICD();
    LNodeTypeMap::iterator itLNodeType = mapLNodeType.find(ln.sLnType);

    if (itLNodeType == mapLNodeType.end())
    {
        pLDdoiCheck->Report(
            pChecker->CalcRowNumberFromOffset(ln.offset),
            pChecker->GetRowContentByOffset(ln.offset),
            "【lnType:%s】在数据类型模板中不存在",
            WtoA(ln.sLnType).c_str());
        return false;
    }
    else
    {
        mapDo = itLNodeType->second.mapDO;
    }
    return true;
}

bool CLDdoiCheck::CheckDOTypeValid_InCacheMgr(DOICache& doi, DOMap& mapDo, DOTypeCache& doType)
{
    CLDdoiCheck* pLDdoiCheck = CLDdoiCheck::GetInstance();

    if (!pLDdoiCheck->IsEnabled())
    {
        return false;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DOTypeMap& mapDoType = pCacheMgr->GetDOTypeCacheOfICD();

    wstring sDoType = mapDo[doi.sname].type;
    DOTypeMap::iterator itDoType = mapDoType.find(sDoType);

    if (sDoType.empty())
    {
        pLDdoiCheck->Report(
            pChecker->CalcRowNumberFromOffset(doi.offset),
            pChecker->GetRowContentByOffset(doi.offset),
            "【doName:%s】在数据类型模板中不存在",
            WtoA(doi.sname).c_str());
        return false;
    }
    else if (itDoType == mapDoType.end())
    {
        pLDdoiCheck->Report(
            pChecker->CalcRowNumberFromOffset(doi.offset),
            pChecker->GetRowContentByOffset(doi.offset),
            "【doType:%s】在数据类型模板中不存在",
            WtoA(sDoType).c_str());
        return false;
    }
    else
    {
        doType = itDoType->second;
    }
    return true;
}

void CLDdoiCheck::CheckDATypeValid_InCacheMgr(int offset, wstring sDaName, SdiCache& sdi)
{
    CLDdoiCheck* pLDdoiCheck = CLDdoiCheck::GetInstance();

    if (!pLDdoiCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DATypeMap& mapDaType = pCacheMgr->GetDATypeCacheOfICD();

    DAMap& mapDa = sdi.mapDaType;
    BDAMap& mapBda = sdi.mapBDA;

    if (sdi.pos == SdiCache::IN_DO && mapDa.end() == mapDa.find(sDaName))
    {
        pLDdoiCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "【daName:%s】在数据类型模板中不存在",
            WtoA(sDaName).c_str());
    }
    else if (sdi.pos == SdiCache::IN_DA && mapBda.end() == mapBda.find(sDaName))
    {
        pLDdoiCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "【daName:%s】在数据类型模板中不存在",
            WtoA(sDaName).c_str());
    }
}


bool CLDdoiCheck::CheckSDITypeValid_InCacheMgr(int offset, wstring sSdiName, SdiCache& sdi)
{
    CLDdoiCheck* pLDdoiCheck = CLDdoiCheck::GetInstance();

    if (!pLDdoiCheck->IsEnabled())
    {
        return false;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DOTypeMap& mapDoType = pCacheMgr->GetDOTypeCacheOfICD();
    DATypeMap& mapDaType = pCacheMgr->GetDATypeCacheOfICD();
    /*
            判断 sdi属性是 sdo对象还是da 属性
    */
    DAMap& mapDa = sdi.mapDaType;
    DOMap& mapDo = sdi.mapSDO;
    BDAMap& mapBda = sdi.mapBDA;

    if (sdi.pos == SdiCache::IN_DO)
    {
        if (mapDa.find(sSdiName) != mapDa.end())
        {
            sdi.pos = SdiCache::IN_DA;
            wstring sDaType = mapDa[sSdiName].type;
            sdi.mapBDA = mapDaType[sDaType].mapBDA;
        }
        else if (mapDo.find(sSdiName) != mapDo.end())
        {
            wstring sDoType = mapDo[sSdiName].type;
            sdi.mapSDO = mapDoType[sDoType].mapSDO;
            sdi.mapDaType = mapDoType[sDoType].mapDaType;
        }
        else
        {
            pLDdoiCheck->Report(
                pChecker->CalcRowNumberFromOffset(offset),
                pChecker->GetRowContentByOffset(offset),
                "【SDI Name:%s】在数据类型模板中不存在",
                WtoA(sSdiName).c_str());
            return false;
        }
    }
    else if (sdi.pos == SdiCache::IN_DA)
    {
        if (mapBda.find(sSdiName) != mapBda.end())
        {
            wstring sBadType = mapBda[sSdiName].type;
            sdi.mapBDA = mapDaType[sBadType].mapBDA;
        }
        else
        {
            pLDdoiCheck->Report(
                pChecker->CalcRowNumberFromOffset(offset),
                pChecker->GetRowContentByOffset(offset),
                "【bdaName:%s】在数据类型模板中不存在",
                WtoA(sSdiName).c_str());
            return false;
        }
    }
    return true;
}