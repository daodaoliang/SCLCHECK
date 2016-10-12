#include "DataSetCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "83395E"

CDataSetCheck::CDataSetCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDataSetTypeOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_IED");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_Services");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DataSet");
    }
}

bool CDataSetCheck::CheckInternal()
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

void CDataSetCheck::CheckName_InCacheMgr(DatasetCache& dateset, int offset)
{
    CDataSetCheck* pDataSetCheck = CDataSetCheck::GetInstance();

    if (!pDataSetCheck->IsEnabled())
    {
        return;
    }
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DataSetTypeMap& mapDataset = pCacheMgr->GetDataSetTypeOfStandard();

    DataSetTypeMap::iterator itDataset = mapDataset.begin();
    while (itDataset != mapDataset.end())
    {
        wstring sNameTemp = itDataset->second.sName;
        int iPos = dateset.sName.find(sNameTemp);
        if (iPos == 0)
        {
            return;
        }
        itDataset++;
    }

    pDataSetCheck->Report(
        pChecker->CalcRowNumberFromOffset(offset),
        pChecker->GetRowContentByOffset(offset),
        "数据集的命名应不符合Q/GDW-1396标准中定义的命名原则(前缀+数字): LDevice inst=%s  DataSet name=%s ",
        WtoA(dateset.sLDevice).c_str(),
        WtoA(dateset.sName).c_str());

}