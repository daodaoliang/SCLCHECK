#include "BayValid1Check.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "06857F"

CBayValid1Check::CBayValid1Check()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("SwitchBay");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Inputs");
    }
}

bool CBayValid1Check::CheckInternal()
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

    SCDCache *pSCDCache = &pCacheMgr->m_SCDCache;
    if (pSCDCache != NULL)
    {
        for (IntervalMap::iterator itIvl = pSCDCache->mapSwitchBay.begin(); itIvl != pSCDCache->mapSwitchBay.end(); itIvl++)
        {
            KeySet *ivlSet = &itIvl->second;
            for (KeySet::iterator itIED = ivlSet->begin(); itIED != ivlSet->end(); itIED++)
            {
                AssoIEDMap::iterator itAsso = pSCDCache->mapAssoIED.find(*itIED);
                if (itAsso != pSCDCache->mapAssoIED.end())
                {
                    for (std::map<wstring, int>::iterator itAIED = itAsso->second.begin(); itAIED != itAsso->second.end(); itAIED++)
                    {
                        wstring ied = itAIED->first;
                        int size = ied.size();
                        wstring interval;
                        if (size > 2 && ied[1] == L'L')
                        {
                            if ((ied[size-1] == L'A') ||
                                (ied[size-1] == L'B'))
                            {
                                interval = ied.substr(2, size-3);
                            }
                            else
                            {
                                interval = ied.substr(2, size-1);
                            }

                            if (interval != itIvl->first)
                            {
                                int row = pChecker->CalcRowNumberFromOffset(itAIED->second);
                                string xml = pChecker->GetRowContentByOffset(itAIED->second);
                                string msg = "位于间隔%s内的IED name=%s与其他线路间隔%s内的IED name=%s存在虚回路";
                                Report(row, xml, msg.c_str(), WtoA(itIvl->first).c_str(), WtoA(*itIED).c_str(), WtoA(interval).c_str(), WtoA(ied).c_str());
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}
