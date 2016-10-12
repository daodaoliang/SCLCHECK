#include "BayValid2Check.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>
#include "../../ScdCheckCommon/deelx.h"

using namespace std;
using namespace com_ximpleware;

#define KEY "EB1230"

CBayValid2Check::CBayValid2Check()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("AllBay");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Inputs");
        CCacheMgr::GetInstance()->AddNeededCacheMark("FCDA");
        CCacheMgr::GetInstance()->AddNeededCacheMark("LogicNodeInstance");
    }
}

bool CBayValid2Check::CheckInternal()
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
    for (KeySet::iterator it = pCacheMgr->m_PMDevice.begin(); it != pCacheMgr->m_PMDevice.end(); it++)
    {
        IEDMap::iterator itIED = pCacheMgr->m_SCDCache.mapIED.find(*it);
        if (itIED != pCacheMgr->m_SCDCache.mapIED.end())
        {
            m_BranchIEDMap.clear();
            IEDCache *pIEDCache = &itIED->second;
            for (LDMap::iterator itLD = pIEDCache->mapLD.begin(); itLD != pIEDCache->mapLD.end(); itLD++)
            {
                LDCache *pLDCache = &itLD->second;
                // GSE���ƿ�
                for (ControlBlockInfoMap::iterator itCB = pLDCache->mapGSEControl.begin(); itCB != pLDCache->mapGSEControl.end(); itCB++)
                {
                    ControlBlockInfoCache *pCBCache = &itCB->second;
                    DataSetMap::iterator itDS = pLDCache->mapDataSet.find(pCBCache->dataSetName);
                    if (itDS != pLDCache->mapDataSet.end())
                    {
                        // �ж�����ź��Ƿ�Ϊ��·
                        DataSetCache *pDSCache = &itDS->second;
                        for (FCDAMap::iterator itFCDA = pDSCache->mapFCDA.begin(); itFCDA != pDSCache->mapFCDA.end(); itFCDA++)
                        {
                            wstring key = pLDCache->iedName + L"." + itFCDA->first;
                            VlinkByOutSignalMap::iterator itOut = pCacheMgr->m_SCDCache.vlinkByOutSignal.find(key);
                            if (itOut != pCacheMgr->m_SCDCache.vlinkByOutSignal.end())
                            {
                                // ȡ���������ж�
                                wstring OutSigDesc;
                                LogicalNodesInstancesMap::iterator itLN = pLDCache->mapSignalInstance.find(itFCDA->first);
                                if (itLN != pLDCache->mapSignalInstance.end())
                                {
                                    LogicalNodesInstanceCache *p_LN = &itLN->second;
                                    if (p_LN->dUVal.size() > 0)
                                    {
                                        OutSigDesc = p_LN->dUVal;
                                    }else
                                    {
                                        OutSigDesc = p_LN->DOIDesc;
                                    }

                                    wstring branch = GetBranch(OutSigDesc);
                                    if (branch.size() > 0)
                                    {
                                        for (KeySet::iterator itInsig = itOut->second.begin(); itInsig != itOut->second.end(); itInsig++)
                                        {
                                            std::vector<wstring> list = Split(*itInsig, L".");
                                            m_BranchIEDMap[branch].insert(list[0]);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                // SV���ƿ�
                for (ControlBlockInfoMap::iterator itCB = pLDCache->mapSampledValueControl.begin(); itCB != pLDCache->mapSampledValueControl.end(); itCB++)
                {
                    ControlBlockInfoCache *pCBCache = &itCB->second;
                    DataSetMap::iterator itDS = pLDCache->mapDataSet.find(pCBCache->dataSetName);
                    if (itDS != pLDCache->mapDataSet.end())
                    {
                        // �ж�����ź��Ƿ�Ϊ��·
                        DataSetCache *pDSCache = &itDS->second;
                        for (FCDAMap::iterator itFCDA = pDSCache->mapFCDA.begin(); itFCDA != pDSCache->mapFCDA.end(); itFCDA++)
                        {
                            wstring key = pLDCache->iedName + L"." + itFCDA->first;
                            VlinkByOutSignalMap::iterator itOut = pCacheMgr->m_SCDCache.vlinkByOutSignal.find(key);
                            if (itOut != pCacheMgr->m_SCDCache.vlinkByOutSignal.end())
                            {
                                // ȡ���������ж�
                                wstring OutSigDesc;
                                LogicalNodesInstancesMap::iterator itLN = pLDCache->mapSignalInstance.find(itFCDA->first);
                                if (itLN != pLDCache->mapSignalInstance.end())
                                {
                                    LogicalNodesInstanceCache *p_LN = &itLN->second;
                                    if (p_LN->dUVal.size() > 0)
                                    {
                                        OutSigDesc = p_LN->dUVal;
                                    }else
                                    {
                                        OutSigDesc = p_LN->DOIDesc;
                                    }

                                    wstring branch = GetBranch(OutSigDesc);
                                    if (branch.size() > 0)
                                    {
                                        for (KeySet::iterator itInsig = itOut->second.begin(); itInsig != itOut->second.end(); itInsig++)
                                        {
                                            std::vector<wstring> list = Split(*itInsig, L".");
                                            m_BranchIEDMap[branch].insert(list[0]);
                                        }
                                    }
                                    
                                }
                            }
                        }
                    }
                }
                // Inputs
                for (InputsMap::iterator itInput = pLDCache->mapInputs.begin(); itInput != pLDCache->mapInputs.end(); itInput++)
                {
                    wstring InSigDesc;
                    InputsCache *pInput = &itInput->second;
                    // �ж������ź�
                    LogicalNodesInstancesMap::iterator itLN = pLDCache->mapSignalInstance.find(pInput->inSignal);
                    if (itLN != pLDCache->mapSignalInstance.end())
                    {
                        LogicalNodesInstanceCache *p_LN = &itLN->second;
                        if (p_LN->dUVal.size() > 0)
                        {
                            InSigDesc = p_LN->dUVal;
                        }else
                        {
                            InSigDesc = p_LN->DOIDesc;
                        }
                        wstring branch = GetBranch(InSigDesc);
                        if (branch.size() > 0)
                        {
                            m_BranchIEDMap[branch].insert(pInput->iedName);
                        }
                    }
                }
            }

            // �жϸ�֧·�Ķ�ӦIED�Ƿ���ͬһ���
            for (IntervalMap::iterator itInterval = m_BranchIEDMap.begin(); itInterval != m_BranchIEDMap.end(); itInterval++)
            {
                KeySet *itKeySet = &itInterval->second; 
                KeySet iedInterval;
                wstring ieds;
                for (KeySet::iterator itKey = itKeySet->begin(); itKey != itKeySet->end(); itKey++)
                {
                    wstring bay = GetIEDInterval(*itKey);
                    ieds += L" IED name=" + *itKey;
                    if (bay.size() > 0)
                    {
                        iedInterval.insert(bay);
                    }

                }
                if (iedInterval.size() > 1)
                {
                    int row = pChecker->CalcRowNumberFromOffset(pIEDCache->offset);
                    string xml = pChecker->GetRowContentByOffset(pIEDCache->offset);
                    string msg = "ĸ�߱�����֧·����Ӧ�ļ����һ�£���ȷ���Ƿ���ȷ:IED name=%s ֧·���� %s �Բ�����װ�� %s";
                    Report(row, xml, msg.c_str(), WtoA(pIEDCache->name).c_str(), WtoA(itInterval->first).c_str(), WtoA(ieds).c_str());
                }
            }

        }
    }

    return true;
}

wstring CBayValid2Check::GetBranch(wstring &desc)
{
    static CRegexpT <wchar_t> regexp(L".*(?<number>֧·[0-9]++).*");
    MatchResult result = regexp.Match(desc.c_str());
    while(result.IsMatched())
    {
        int gN = regexp.GetNamedGroupNumber(L"number");
        if (gN >= 0)
        {
            int len = result.GetGroupEnd(gN) - result.GetGroupStart(gN);
            return desc.substr(result.GetGroupStart(gN), len);
        }
        result = regexp.Match(desc.c_str(), result.GetEnd());
    }
    return L"";
}

wstring CBayValid2Check::GetIEDInterval(wstring iedName)
{
    static CRegexpT <wchar_t> regexp(L"[A-Z](?<Bay>[A-Z][0-9]+)");
    MatchResult result = regexp.Match(iedName.c_str());
    while(result.IsMatched())
    {
        int gN = regexp.GetNamedGroupNumber(L"Bay");
        if (gN >= 0)
        {
            int len = result.GetGroupEnd(gN) - result.GetGroupStart(gN);
            return iedName.substr(result.GetGroupStart(gN), len);
        }
        result = regexp.Match(iedName.c_str(), result.GetEnd());
    }
    return L"";
}
