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

    // ȡ����Ŀ��������Ϣ�����õ�������
    if (CConfigMgr::GetInstance()->GetItemConfig(KEY, cfg))
    {
        SetConfig(cfg);
    }

    // ��������˱���Ŀ����������ע��
    if (IsEnabled())
    {
        CScdChecker::GetInstance()->RegisterCheckItem(this);

        // �ڴ˴������Ҫĳĳ���͵Ļ�����Ϣ�������Լ��������Ŀ
        CCacheMgr::GetInstance()->AddNeededCacheMark("SwitchBay");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Inputs");
    }
}

bool CBayValid1Check::CheckInternal()
{
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    // ���������
    COutputMgr *pOutputMgr = COutputMgr::GetInstance();
    // �����ļ�������
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
                                string msg = "λ�ڼ��%s�ڵ�IED name=%s��������·���%s�ڵ�IED name=%s�������·";
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
