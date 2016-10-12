#include "FcdaValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>
#include <atlstr.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "EA5BBA"

CFcdaValidCheck::CFcdaValidCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_IED");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DataSet");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_FCDA");

        CCacheMgr::GetInstance()->AddNeededCacheMark("GetLNodeTypeCacheOfICD");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfICD");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDATypeCacheOfICD");
    }
}

bool CFcdaValidCheck::CheckInternal()
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

/*
检测逻辑:
通过Fcda的特定属性定位到LNType
通过Fcda的doName属性定位到DOType
*/
bool CFcdaValidCheck::CheckValid_InCacheMgr(FcdaCache& fcda, LDeviceCache& lDevice)
{
    CFcdaValidCheck* pFcdaValidCheck = CFcdaValidCheck::GetInstance();

    if (!pFcdaValidCheck->IsEnabled())
    {
        return true;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    LNodeTypeMap& mapLNodeType = pCacheMgr->GetLNodeTypeCacheOfICD();

    wstring sLNodeTypeKey = fcda.sPrefix + fcda.sLnClass + fcda.sLnInst;
    wstring sLNodeType = lDevice.mapLN[sLNodeTypeKey].sLnType;
    if (sLNodeType.empty())
    {
        return false;
        pFcdaValidCheck->Report(
            pChecker->CalcRowNumberFromOffset(fcda.offset),
            pChecker->GetRowContentByOffset(fcda.offset),
            "%s",
            "未找到FCDA引用的LNodeType");
    }
    LNodeTypeCache& lNodeType = mapLNodeType[sLNodeType];
    if (lNodeType.id.empty())
    {
        return false;
        pFcdaValidCheck->Report(
            pChecker->CalcRowNumberFromOffset(fcda.offset),
            pChecker->GetRowContentByOffset(fcda.offset),
            "FCDA引用的【LNodeType:%s】在数据类型模板中不存在",
            WtoA(sLNodeType).c_str());
    }

    vector<wstring>& vec = split_string(fcda.sDoName + L"." + fcda.sDaName, L".");
    DOMap& mapDo = lNodeType.mapDO;
    DOMap::iterator itDo = mapDo.find(vec[0]);
    if (itDo != mapDo.end())
    {
        return CheckDoType(fcda.offset, vector<wstring>(vec.begin()+1,vec.end()),itDo->second.type);
    }
    else
    {
        return false;
        pFcdaValidCheck->Report(
            pChecker->CalcRowNumberFromOffset(fcda.offset),
            pChecker->GetRowContentByOffset(fcda.offset),
            "FCDA的【doName:%s】引用无效",
            WtoA(vec[0]).c_str());
    }
}

bool CFcdaValidCheck::CheckDoType(int offset, vector<wstring>& vecDoName, wstring sDoType)
{
    if (vecDoName.size() == 0)
    {
        return true;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    CFcdaValidCheck* pFcdaValidCheck = CFcdaValidCheck::GetInstance();

    DOTypeMap& mapDoType = pCacheMgr->GetDOTypeCacheOfICD();

    wstring sDoName = vecDoName[0];
    DOTypeMap::iterator itDoType = mapDoType.find(sDoType);
    if (itDoType != mapDoType.end())
    {
        DAMap& mapDa = itDoType->second.mapDaType;
        DOMap& mapSdo = itDoType->second.mapSDO;
        DOMap::iterator itDo = mapSdo.find(sDoName);
        if (itDo != mapSdo.end())
        {
            return CheckDoType(offset, vector<wstring>(vecDoName.begin()+1,vecDoName.end()),itDo->second.type);
        }
        else
        {
            DAMap::iterator itDa = mapDa.find(sDoName);
            if (itDa != mapDa.end())
            {
                return CheckDaType(offset, vector<wstring>(vecDoName.begin()+1,vecDoName.end()), itDa->second.type);
            }
            else
            {
                return false;
                pFcdaValidCheck->Report(
                    pChecker->CalcRowNumberFromOffset(offset),
                    pChecker->GetRowContentByOffset(offset),
                    "FCDA的【doName:%s】引用无效",
                    WtoA(sDoName).c_str());
            }
        }
    }
    else
    {
        return false;
        pFcdaValidCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "FCDA的【doType:%s】引用无效",
            WtoA(sDoType).c_str());
    }
    return false;
}

bool CFcdaValidCheck::CheckDaType(int offset, vector<wstring>& vecDaName, wstring sDaType)
{
    if (vecDaName.size() == 0)
    {
        return true;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    CFcdaValidCheck* pFcdaValidCheck = CFcdaValidCheck::GetInstance();

    DATypeMap& mapDaType = pCacheMgr->GetDATypeCacheOfICD();
    wstring sDaName = vecDaName[0];
    DATypeMap::iterator itDaType = mapDaType.find(sDaType);
    if (itDaType != mapDaType.end())
    {
        BDAMap& mapBda = itDaType->second.mapBDA;
        BDAMap::iterator itBda = mapBda.find(sDaName);
        if (itBda != mapBda.end())
        {
            return CheckDaType(offset, vector<wstring>(vecDaName.begin()+1, vecDaName.end()), itBda->second.type);
        }
        else
        {
            return false;
            pFcdaValidCheck->Report(
                pChecker->CalcRowNumberFromOffset(offset),
                pChecker->GetRowContentByOffset(offset),
                "FCDA的【daName:%s】引用无效",
                WtoA(sDaName).c_str());
        }
    }
    else
    {
        return false;
        pFcdaValidCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "FCDA的【daType:%s】引用无效",
            WtoA(sDaType).c_str());
    }
    return false;
}