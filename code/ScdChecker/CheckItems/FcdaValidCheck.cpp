#include "FcdaValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "A63491"

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
        CCacheMgr::GetInstance()->AddNeededCacheMark("FCDA");
        CCacheMgr::GetInstance()->AddNeededCacheMark("DataTypeTemplates");
    }
}

// 数据集配置的FCDA非空的属性值是否指向模型中已存在的数据对象和数据属性
bool CFcdaValidCheck::CheckInternal()
{
    //// 检测器
    //CScdChecker *pChecker = CScdChecker::GetInstance();
    //// 缓存管理器器
    //CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    //// 输出管理器
    //COutputMgr *pOutputMgr = COutputMgr::GetInstance();
    //// 配置文件管理器
    //CConfigMgr *pConfigMgr = CConfigMgr::GetInstance();

    //assert(pChecker != NULL);
    //assert(pCacheMgr != NULL);
    //assert(pOutputMgr != NULL);
    //assert(pConfigMgr != NULL);

    //SCDCache *p_SCDCache = &pCacheMgr->m_SCDCache;
    //if (NULL != p_SCDCache)
    //{
    //    for (IEDMap::iterator itIED = p_SCDCache->mapIED.begin(); itIED != p_SCDCache->mapIED.end(); itIED++)
    //    {
    //        IEDCache *p_IED = &itIED->second;
    //        for (LDMap::iterator itLD = p_IED->mapLD.begin(); itLD != p_IED->mapLD.end(); itLD++)
    //        {
    //            LDCache *p_LD = &itLD->second;
    //            for (DataSetMap::iterator itDS = p_LD->mapDataSet.begin(); itDS != p_LD->mapDataSet.end(); itDS++)
    //            {
    //                DataSetCache *p_DS = &itDS->second;
    //                for (FCDAMap::iterator itFCDA = p_DS->mapFCDA.begin(); itFCDA !=  p_DS->mapFCDA.end(); itFCDA++)
    //                {
    //                    FCDACache *p_FCDA = &itFCDA->second;
    //                    LogicalNodesInstancesMap::iterator itInstace = p_LD->mapSignalInstance.find(itFCDA->first);
    //                    if (itInstace == p_LD->mapSignalInstance.end())
    //                    {
    //                        int row = pChecker->CalcRowNumberFromOffset(p_FCDA->offset);
    //                        string xml = pChecker->GetRowContentByOffset(p_FCDA->offset);
    //                        string msg = "数据集配置的FCDA所指向的模型中不存在对应的数据对象和数据属性: IED name=%s, LDevice inst=%s, DataSet name=%s, FCDA=%s";
    //                        Report(row, xml, msg.c_str(), WtoA(p_IED->name).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_DS->name).c_str(), WtoA(p_FCDA->key).c_str());
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}

    return true;
}
