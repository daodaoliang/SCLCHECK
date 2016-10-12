#include "DataSetTypeValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "EA80EF"

CDataSetTypeValidCheck::CDataSetTypeValidCheck()
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
    }
}

bool CDataSetTypeValidCheck::CheckInternal()
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

void CDataSetTypeValidCheck::CheckValid_InCacheMgr(wstring sCBType, FcdaCache& fcda)
{
    CDataSetTypeValidCheck* pDataSetTypeValidCheck = CDataSetTypeValidCheck::GetInstance();

    if (!pDataSetTypeValidCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    wstring sKey = fcda.sLdInst + L"/" + fcda.sPrefix + fcda.sLnClass + fcda.sLnInst + L"." + fcda.sDoName;
    if (!fcda.sDaName.empty())
    {
        sKey += L".";
        sKey += fcda.sDaName;
    }

    if(sCBType == L"GSEControl")
    {
        if(!fcda.bHasDoName || !fcda.bHasDaName)
        {
            pDataSetTypeValidCheck->Report(
                pChecker->CalcRowNumberFromOffset(fcda.offset),
                pChecker->GetRowContentByOffset(fcda.offset),
                "GOOSE数据集未配置到DA：LDevice inst=%s, DataSet name=%s, FCDA=%s",
                WtoA(fcda.sLdInst).c_str(),
                WtoA(fcda.sDataset).c_str(),
                WtoA(sKey).c_str());
        }
    }
    else
    {
        if(!fcda.bHasDoName || fcda.bHasDaName)
        {
            pDataSetTypeValidCheck->Report(
                pChecker->CalcRowNumberFromOffset(fcda.offset),
                pChecker->GetRowContentByOffset(fcda.offset),
                "SV数据集未配置到DO：LDevice inst=%s, DataSet name=%s, FCDA=%s",
                WtoA(fcda.sLdInst).c_str(),
                WtoA(fcda.sDataset).c_str(),
                WtoA(sKey).c_str());
        }
    }
}
