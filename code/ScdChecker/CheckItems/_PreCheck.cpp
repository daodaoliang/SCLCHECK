#include "_PreCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"

#include "IpUniqueCheck.h"
#include "InputsDescValid2Check.h"
#include "ControlIdUniqueCheck.h"
#include "InputsFormatCheck.h"
#include "CommunicationRedunCheck.h"
#include "CommunicationValidCheck.h"
#include "GseMacValidCheck.h"
#include "InputsDescValid1Check.h"
#include "InputsDtDODAValidCheck.h"
#include "InputsDtCDCValidCheck.h"
#include "InputsDtbTypeValidCheck.h"
#include "IpValidCheck.h"
#include "LDescBlankCheck.h"
#include "GseAppidValidCheck.h"
#include "SmvAppidValidCheck.h"
#include "DaTempValidCheck.h"
#include "DataTempRedunCheck.h"
#include "DoiDescBlankCheck.h"
#include "InputsStadValidCheck.h"
#include "VlanValidCheck.h"
#include "BayValid2Check.h"
#include "ControlBlockLdCheck.h"
#include "ConnectedAPValidCheck.h"
#include "InputsValidCheck.h"
#include "SVControlBlockInfoCheck.h"
#include "GSEControlBlockInfoCheck.h"
#include "FcdaValidCheck.h"

#include <assert.h>

using namespace std;
using namespace com_ximpleware;

CPreCheck::CPreCheck()
{
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    p_SCDCache = &pCacheMgr->m_SCDCache;
}

bool CPreCheck::CheckCommon()
{
    pChecker = CScdChecker::GetInstance();
    ConnectedAPValidCheck();
    ControlBlockCheck();
    InputsCheck();
    DoiInstanceCheck();
    FcdaValidCheck();
    DaTempValidCheck();
    DataTempRedunCheck();
    return true;
}

// ���ƿ�������ݼ��Ƿ���ͬһLD���ҵ�
bool CPreCheck::ControlBlockCheck()
{
    if (CControlBlockLdCheck::GetInstance()->IsEnabled() || 
        CCommunicationValidCheck::GetInstance()->IsEnabled() ||
        CCommunicationRedunCheck::GetInstance()->IsEnabled() ||
        CSVControlBlockInfoCheck::GetInstance()->IsEnabled() ||
        CGSEControlBlockInfoCheck::GetInstance()->IsEnabled())
    {
        if (p_SCDCache != NULL)
        {
            for (IEDMap::iterator itIED = p_SCDCache->mapIED.begin(); itIED != p_SCDCache->mapIED.end(); itIED++)
            {
                IEDCache *p_IEDCache = &itIED->second;

                for (LDMap::iterator itLD = p_IEDCache->mapLD.begin(); itLD != p_IEDCache->mapLD.end(); itLD++)
                {
                    LDCache *p_LD = &itLD->second;
                    for (ControlBlockInfoMap::iterator it = p_LD->mapGSEControl.begin(); it != p_LD->mapGSEControl.end(); it++)
                    {
                        ControlBlockInfoCache *p_CBInfo = &it->second;
                        ControlBlockLdCheck(p_LD, p_CBInfo);
                        CommunicationValidCheck(p_LD, p_CBInfo, L"GOOSE");
                    }
                    for (ControlBlockInfoMap::iterator it = p_LD->mapSampledValueControl.begin(); it != p_LD->mapSampledValueControl.end(); it++)
                    {
                        ControlBlockInfoCache *p_CBInfo = &it->second;
                        ControlBlockLdCheck(p_LD, p_CBInfo);
                        CommunicationValidCheck(p_LD, p_CBInfo, L"SV");
                    }
                    for (ControlBlockInfoMap::iterator it = p_LD->mapReportControl.begin(); it != p_LD->mapReportControl.end(); it++)
                    {
                        ControlBlockInfoCache *p_CBInfo = &it->second;
                        ControlBlockLdCheck(p_LD, p_CBInfo);
                    }
                    for (ControlBlockInfoMap::iterator it = p_LD->mapLogControl.begin(); it != p_LD->mapLogControl.end(); it++)
                    {
                        ControlBlockInfoCache *p_CBInfo = &it->second;
                        ControlBlockLdCheck(p_LD, p_CBInfo);
                    }
                }
            }
        }
    }
    return true;
}

bool CPreCheck::ControlBlockLdCheck(LDCache *p_LD, ControlBlockInfoCache *p_CBInfo)
{
    if (CControlBlockLdCheck::GetInstance()->IsEnabled())
    {

        DataSetMap::iterator itDs = p_LD->mapDataSet.find(p_CBInfo->dataSetName);
        if (itDs == p_LD->mapDataSet.end())
        {
            int row = pChecker->CalcRowNumberFromOffset(p_CBInfo->offset);
            string xml = pChecker->GetRowContentByOffset(p_CBInfo->offset);
            string msg = "���ƿ���������ݼ�δ��ͬһLD���ҵ�: IED name=%s LDevice inst=%s ControlBlock name=%s";
            CControlBlockLdCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_CBInfo->name).c_str());
        }
    }
    
    return true;
}

bool CPreCheck::CommunicationValidCheck(LDCache *p_LD, ControlBlockInfoCache *p_CBInfo, wstring type)
{
    if (CCommunicationValidCheck::GetInstance()->IsEnabled() ||
        CCommunicationRedunCheck::GetInstance()->IsEnabled() ||
        CSVControlBlockInfoCheck::GetInstance()->IsEnabled() ||
        CGSEControlBlockInfoCheck::GetInstance()->IsEnabled())
    {

        if (p_CBInfo->appID.size() <= 0)
        {
            if ((type == L"GOOSE") && (CGSEControlBlockInfoCheck::GetInstance()->IsEnabled()))
            {
                int row = pChecker->CalcRowNumberFromOffset(p_CBInfo->offset);
                string xml = pChecker->GetRowContentByOffset(p_CBInfo->offset);
                string msg = "GSE���ƿ��������GOID��appID��ȱʧ:IED name=%s AccessPoint name=%s LD inst=%s GSEControl name=%s";
                CGSEControlBlockInfoCheck::GetInstance()->Report(row, xml, msg.c_str(), WtoA(p_LD->iedName).c_str(), WtoA(p_LD->apName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_CBInfo->name).c_str());
            }else if ((type == L"SV") && (CGSEControlBlockInfoCheck::GetInstance()->IsEnabled()))
            {
                int row = pChecker->CalcRowNumberFromOffset(p_CBInfo->offset);
                string xml = pChecker->GetRowContentByOffset(p_CBInfo->offset);
                string msg = "SV���ƿ��������smvIDȱʧ:IED name=%s AccessPoint name=%s LD inst=%s GSEControl name=%s";
                CSVControlBlockInfoCheck::GetInstance()->Report(row, xml, msg.c_str(), WtoA(p_LD->iedName).c_str(), WtoA(p_LD->apName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_CBInfo->name).c_str());
            }
        }

        bool isFound = false; // ����ͨ������
        bool isVirtualLinked = false; // �Ƿ���ڶ��Ĺ�ϵ
        for (SubNetWorkMap::iterator itSubNet = p_SCDCache->mapSubNet.begin(); itSubNet != p_SCDCache->mapSubNet.end(); itSubNet++)
        {
            SubNetWorkCache *p_SubNet = &itSubNet->second;
            wstring conAPKey = p_LD->iedName + L"." + p_LD->apName;
            ConnectedAPMap::iterator itConAP = p_SubNet->mapConnectedAPMap.find(conAPKey);
            if (itConAP != p_SubNet->mapConnectedAPMap.end())
            {
                ConnectedAPCache *p_ConAp = &itConAP->second;
                wstring cbKey = p_LD->inst + L"." + p_CBInfo->name;
                ControlBlockAddressMap::iterator itGseCBAddr = p_ConAp->mapGseCBAddress.find(cbKey);
                if (itGseCBAddr != p_ConAp->mapGseCBAddress.end())
                {
                    ControlBlockAddressCache *pCBAddressCache = &itGseCBAddr->second;
                    if ((CGSEControlBlockInfoCheck::GetInstance()->IsEnabled()) &&
                        (!((pCBAddressCache->APPID.size() > 0) &&
                          (pCBAddressCache->VLANID.size() > 0) &&
                          (pCBAddressCache->VLANPRIORITY.size() > 0) &&
                          (pCBAddressCache->MACAddress.size() > 0) &&
                          (pCBAddressCache->MAXTime.size() > 0) &&
                          (pCBAddressCache->MinTime.size() > 0))))
                    {
                        int row = pChecker->CalcRowNumberFromOffset(p_CBInfo->offset);
                        string xml = pChecker->GetRowContentByOffset(p_CBInfo->offset);
                        string msg = "GSE���ƿ��������GOID��appID����APPID��MAC��ַ��VLAN-ID�����ȼ���MaxTime��MinTime����ȱʧ��:IED name=%s AccessPoint name=%s LD inst=%s GSEControl name=%s";
                        CGSEControlBlockInfoCheck::GetInstance()->Report(row, xml, msg.c_str(), WtoA(p_LD->iedName).c_str(), WtoA(p_LD->apName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_CBInfo->name).c_str());
                    }
                    isFound = true;
                    break;
                }
                ControlBlockAddressMap::iterator itSmvCBAddr = p_ConAp->mapSmvCBAddress.find(cbKey);
                if (itSmvCBAddr != p_ConAp->mapSmvCBAddress.end())
                {
                    ControlBlockAddressCache *pCBAddressCache = &itSmvCBAddr->second;
                    if ((CSVControlBlockInfoCheck::GetInstance()->IsEnabled()) &&
                        (!((pCBAddressCache->APPID.size() > 0) &&
                         (pCBAddressCache->VLANID.size() > 0) &&
                         (pCBAddressCache->VLANPRIORITY.size() > 0) &&
                         (pCBAddressCache->MACAddress.size() > 0))))
                    {
                        int row = pChecker->CalcRowNumberFromOffset(p_CBInfo->offset);
                        string xml = pChecker->GetRowContentByOffset(p_CBInfo->offset);
                        string msg = "SV���ƿ��������smvID��APPID��MAC��ַ��VLAN-ID����ȱʧ��:IED name=%s AccessPoint name=%s LD inst=%s GSEControl name=%s";
                        CSVControlBlockInfoCheck::GetInstance()->Report(row, xml, msg.c_str(), WtoA(p_LD->iedName).c_str(), WtoA(p_LD->apName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_CBInfo->name).c_str());
                    }
                    isFound = true;
                    break;
                }
            }
        }
        if (!isFound)
        {
            DataSetMap::iterator itDS = p_LD->mapDataSet.find(p_CBInfo->dataSetName);
            if (itDS != p_LD->mapDataSet.end())
            {
                DataSetCache *p_DS = &itDS->second;
                for(FCDAMap::iterator itFCDA = p_DS->mapFCDA.begin(); itFCDA != p_DS->mapFCDA.end(); itFCDA++)
                {
                    wstring outKey = p_LD->iedName + L"." + itFCDA->first;
                    VlinkByOutSignalMap::iterator itOut = p_SCDCache->vlinkByOutSignal.find(outKey);
                    if (itOut != p_SCDCache->vlinkByOutSignal.end())
                    {
                        isVirtualLinked = true;
                        break;
                    }
                }
            }

            if (isVirtualLinked)
            {
                if (CCommunicationValidCheck::GetInstance()->IsEnabled())
                {
                    int row = pChecker->CalcRowNumberFromOffset(p_CBInfo->offset);
                    string xml = pChecker->GetRowContentByOffset(p_CBInfo->offset);
                    string msg = "SCD�ļ��д��ڶ��Ĺ�ϵ������ӣ���Ӧ���Ϳ��ƿ��ͨ�Ų������ò�����:IED name=%s LDevice inst=%s ControlBlock name=%s";
                    CCommunicationValidCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                        WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_CBInfo->name).c_str());
                }
            }
            else
            {
                if (CCommunicationRedunCheck::GetInstance()->IsEnabled())
                {
                    int row = pChecker->CalcRowNumberFromOffset(p_CBInfo->offset);
                    string xml = pChecker->GetRowContentByOffset(p_CBInfo->offset);
                    string msg = "GOOSE��SVδ�����ģ���ͨ�Ų���δ����:IED name=%s LDevice inst=%s ControlBlock name=%s";
                    CCommunicationRedunCheck::GetInstance()->Report(row, xml, msg.c_str(),
                        WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_CBInfo->name).c_str());
                }
            }
        }
    }
    return true;
}

bool CPreCheck::FcdaValidCheck()
{
    if (!CFcdaValidCheck::GetInstance()->IsEnabled())
    {
        return true;
    }

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

    SCDCache *p_SCDCache = &pCacheMgr->m_SCDCache;
    if (NULL != p_SCDCache)
    {
        for (IEDMap::iterator itIED = p_SCDCache->mapIED.begin(); itIED != p_SCDCache->mapIED.end(); itIED++)
        {
            IEDCache *p_IED = &itIED->second;
            for (LDMap::iterator itLD = p_IED->mapLD.begin(); itLD != p_IED->mapLD.end(); itLD++)
            {
                LDCache *p_LD = &itLD->second;
                for (DataSetMap::iterator itDS = p_LD->mapDataSet.begin(); itDS != p_LD->mapDataSet.end(); itDS++)
                {
                    DataSetCache *p_DS = &itDS->second;
                    for (FCDAMap::iterator itFCDA = p_DS->mapFCDA.begin(); itFCDA !=  p_DS->mapFCDA.end(); itFCDA++)
                    {
                        FCDACache *p_FCDA = &itFCDA->second;
                        wstring ln = p_FCDA->prefix + p_FCDA->lnClass + p_FCDA->lnInst;
                        LogicalNodeMap::iterator  itLN = p_LD->mapLogicalNode.find(ln);
                        if (itLN == p_LD->mapLogicalNode.end())
                        {
                            int row = pChecker->CalcRowNumberFromOffset(p_FCDA->offset);
                            string xml = pChecker->GetRowContentByOffset(p_FCDA->offset);
                            string msg = "���ݼ����õ�FCDA��ָ���ģ���в����ڶ�Ӧ��LNClass: IED name=%s, LDevice inst=%s, DataSet name=%s, FCDA=%s";
                            CFcdaValidCheck::GetInstance()->Report(row, xml, msg.c_str(), WtoA(p_IED->name).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_DS->name).c_str(), WtoA(p_FCDA->key).c_str());
                        }
                        else
                        {
                            LogicalNodeCache *p_LN = &itLN->second;
                            wstring type; 
                            bool bToDo = false;
                            if (!CheckLNodeType(p_LD->iedName, p_LN->lnType, p_FCDA->doName, p_FCDA->daName, type, bToDo))
                            {
                                int row = pChecker->CalcRowNumberFromOffset(p_FCDA->offset);
                                string xml = pChecker->GetRowContentByOffset(p_FCDA->offset);
                                string msg = "���ݼ����õ�FCDA��ָ���ģ���в����ڶ�Ӧ�����ݶ������������: IED name=%s, LDevice inst=%s, DataSet name=%s, FCDA=%s";
                                CFcdaValidCheck::GetInstance()->Report(row, xml, msg.c_str(), WtoA(p_IED->name).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_DS->name).c_str(), WtoA(p_FCDA->key).c_str());
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

// Communication��ConnectedAP��iedName��apName�Լ�GSE\SMV��cbName��ldInst������ֵ�Ƿ���IED�д�����Ӧ��Ϣ
bool CPreCheck::ConnectedAPValidCheck()
{
    if (CConnectedAPValidCheck::GetInstance()->IsEnabled())
    {

        if (p_SCDCache != NULL)
        {
            for (SubNetWorkMap::iterator itSubNet = p_SCDCache->mapSubNet.begin(); itSubNet != p_SCDCache->mapSubNet.end(); itSubNet++)
            {
                SubNetWorkCache *p_SubNet = &itSubNet->second;
                for (ConnectedAPMap::iterator itConAp = p_SubNet->mapConnectedAPMap.begin(); itConAp != p_SubNet->mapConnectedAPMap.end(); itConAp++)
                {
                    ConnectedAPCache *p_ConAP = &itConAp->second;
                    // �ж���IED���Ƿ����
                    IEDMap::iterator itIED = p_SCDCache->mapIED.find(p_ConAP->iedName);
                    if (itIED != p_SCDCache->mapIED.end())
                    {
                        IEDCache *p_IED = &itIED->second;
                        if (p_IED->apSet.count(p_ConAP->apName) == 1)
                        {
                            // �˴�����mapGseCBAddress�е�CB��ΪGse���ƿ�
                            for (ControlBlockAddressMap::iterator itCBAddr = p_ConAP->mapGseCBAddress.begin(); 
                                itCBAddr != p_ConAP->mapGseCBAddress.end(); itCBAddr++)
                            {
                                ControlBlockAddressCache *p_CBAddr = &itCBAddr->second;
                                wstring key = p_ConAP->iedName + L"." + p_ConAP->apName + L"." + p_CBAddr->ldInst;

                                LDMap::iterator itLD = p_IED->mapLD.find(key);
                                if (itLD != p_IED->mapLD.end())
                                {
                                    LDCache *p_LD = &itLD->second;
                                    ControlBlockInfoMap::iterator itCBInfo = p_LD->mapGSEControl.find(p_CBAddr->cbName);
                                    if (itCBInfo == p_LD->mapGSEControl.end())
                                    {
                                        int row = pChecker->CalcRowNumberFromOffset(p_CBAddr->offset);
                                        string xml = pChecker->GetRowContentByOffset(p_CBAddr->offset);
                                        string msg = "Communication��ConnectedAP��GSE��cbName����ֵδָ���Ѵ��ڵ�IED���ʵ���LD�Ŀ��ƿ�: SubNetwork name=%s, ConnectedAP iedName=%s apName=%s, GSE ldInst=%s cbName=%s";
                                        CConnectedAPValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                                            WtoA(p_SubNet->subNetWorkName).c_str(), WtoA(p_ConAP->iedName).c_str(), WtoA(p_ConAP->apName).c_str(), WtoA(p_CBAddr->ldInst).c_str(), WtoA(p_CBAddr->cbName).c_str());
                                    }
                                }
                                else
                                {
                                    int row = pChecker->CalcRowNumberFromOffset(p_CBAddr->offset);
                                    string xml = pChecker->GetRowContentByOffset(p_CBAddr->offset);
                                    string msg = "Communication��ConnectedAP��GSE��ldInst����ֵδָ���Ѵ��ڵ�IED���ʵ��µ�LD:SubnetWork name=%s ConnectedAP iedName=%s apName=%s GSE ldInst=%s cbName=%s";
                                    CConnectedAPValidCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                                        WtoA(p_SubNet->subNetWorkName).c_str(), WtoA(p_ConAP->iedName).c_str(), WtoA(p_ConAP->apName).c_str(), WtoA(p_CBAddr->ldInst).c_str(), WtoA(p_CBAddr->cbName).c_str());
                                }
                            }
                            // �˴�����mapSmvCBAddress�е�CB��ΪSV���ƿ�
                            for (ControlBlockAddressMap::iterator itCBAddr = p_ConAP->mapSmvCBAddress.begin(); 
                                itCBAddr != p_ConAP->mapSmvCBAddress.end(); itCBAddr++)
                            {
                                ControlBlockAddressCache *p_CBAddr = &itCBAddr->second;
                                wstring key = p_ConAP->iedName + L"." + p_ConAP->apName + L"." + p_CBAddr->ldInst;

                                LDMap::iterator itLD = p_IED->mapLD.find(key);
                                if (itLD != p_IED->mapLD.end())
                                {
                                    LDCache *p_LD = &itLD->second;
                                    ControlBlockInfoMap::iterator itCBInfo = p_LD->mapSampledValueControl.find(p_CBAddr->cbName);
                                    if (itCBInfo == p_LD->mapSampledValueControl.end())
                                    {
                                        int row = pChecker->CalcRowNumberFromOffset(p_CBAddr->offset);
                                        string xml = pChecker->GetRowContentByOffset(p_CBAddr->offset);
                                        string msg = "Communication��ConnectedAP��SMV��cbName����ֵδָ���Ѵ��ڵ�IED���ʵ���LD�Ŀ��ƿ�:SubNetwork name=%s, ConnectedAP iedName=%s apName=%s, SMV ldInst=%s cbName=%s";
                                        CConnectedAPValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                                            WtoA(p_SubNet->subNetWorkName).c_str(), WtoA(p_ConAP->iedName).c_str(), WtoA(p_ConAP->apName).c_str(), WtoA(p_CBAddr->ldInst).c_str(), WtoA(p_CBAddr->cbName).c_str());
                                    }
                                }
                                else
                                {
                                    int row = pChecker->CalcRowNumberFromOffset(p_CBAddr->offset);
                                    string xml = pChecker->GetRowContentByOffset(p_CBAddr->offset);
                                    string msg = "Communication��ConnectedAP��SMV��ldInst����ֵδָ���Ѵ��ڵ�IED���ʵ��µ�LD:SubNetwork name=%s, ConnectedAP iedName=%s apName=%s, GSE ldInst=%s cbName=%s";
                                    CConnectedAPValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                                        WtoA(p_SubNet->subNetWorkName).c_str(), WtoA(p_ConAP->iedName).c_str(), WtoA(p_ConAP->apName).c_str(), WtoA(p_CBAddr->ldInst).c_str(), WtoA(p_CBAddr->cbName).c_str());
                                }
                            }
                        }
                        else
                        {
                            int row = pChecker->CalcRowNumberFromOffset(p_ConAP->offset);
                            string xml = pChecker->GetRowContentByOffset(p_ConAP->offset);
                            string msg = "Communication��ConnectedAP��apName����ֵδָ���Ѵ��ڵ�IED�ķ��ʵ�:SubNetwork name=%s, ConnectedAP iedName=%s apName=%s";
                            CConnectedAPValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                                WtoA(p_SubNet->subNetWorkName).c_str(), WtoA(p_ConAP->iedName).c_str(), WtoA(p_ConAP->apName).c_str());
                        }
                    }
                    else
                    {
                        int row = pChecker->CalcRowNumberFromOffset(p_ConAP->offset);
                        string xml = pChecker->GetRowContentByOffset(p_ConAP->offset);
                        string msg = "Communication��ConnectedAP��iedName����ֵδָ���Ѵ��ڵ�IED:SubNetwork name=%s, ConnectedAP iedName=%s apName=%s";
                        CConnectedAPValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                            WtoA(p_SubNet->subNetWorkName).c_str(), WtoA(p_ConAP->iedName).c_str(), WtoA(p_ConAP->apName).c_str());
                    }
                }
            }
        }
    }

    return true;
}

bool CPreCheck::InputsCheck()
{
    if (!CInputsValidCheck::GetInstance()->IsEnabled() &&
        !CInputsDtDODAValidCheck::GetInstance()->IsEnabled() &&
        !CInputsDtCDCValidCheck::GetInstance()->IsEnabled() &&
        !CInputsDtbTypeValidCheck::GetInstance()->IsEnabled() &&
        !CInputsDescValid1Check::GetInstance()->IsEnabled() &&
        !CInputsDescValid2Check::GetInstance()->IsEnabled())
    {
        return true;
    }
    if (p_SCDCache != NULL)
    {
        for (IEDMap::iterator itIED = p_SCDCache->mapIED.begin(); itIED != p_SCDCache->mapIED.end(); itIED++)
        {
            IEDCache *p_IEDCache = &itIED->second;
            for (LDMap::iterator itLD = p_IEDCache->mapLD.begin(); itLD != p_IEDCache->mapLD.end(); itLD++)
            {
                LDCache *p_LD = &itLD->second;
                for (InputsMap::iterator itIn = p_LD->mapInputs.begin(); itIn != p_LD->mapInputs.end(); itIn++)
                {
                    InputsCache *p_In = &itIn->second;
                    // SCD�ļ�����������ߵĶ��������Ƿ���Ч��
                    //if (!InputsFormatCheck(itLD->second, p_In))
                    //{
                        InputsValidCheck(p_LD, p_In);
                        InputsDtValidCheck(p_LD, p_In);
                        InputsDescValidCheck(p_LD, p_In);
                    //}
                }
            }
        }
    }
    return true;
}

// SCD�ļ�����������ߵĶ��������Ƿ���Ч��
bool CPreCheck::InputsValidCheck(LDCache *p_LD, InputsCache *p_In)
{
    if (CInputsValidCheck::GetInstance()->IsEnabled())
    {
        CConfigMgr *pConfigMgr = CConfigMgr::GetInstance();
        assert(pConfigMgr != NULL);

        // �ж������ź�������˿��Ƿ���Ч
        if (p_In->Port != L"")
        {
            std::vector<wstring> portList = Split(p_In->Port, L"/");
            for (int i = 0; i < portList.size(); i++)
            {
                bool bFoundPort = false;
                for (SubNetWorkMap::iterator itSubNet = p_SCDCache->mapSubNet.begin(); itSubNet != p_SCDCache->mapSubNet.end(); itSubNet++)
                {
                    SubNetWorkCache *pSubNet = &itSubNet->second;
                    for (ConnectedAPMap::iterator itConAP = pSubNet->mapConnectedAPMap.begin(); itConAP != pSubNet->mapConnectedAPMap.end(); itConAP++)
                    {
                        ConnectedAPCache *pConAp = &itConAP->second;
                        if ((pConAp->iedName == p_LD->iedName) && (pConAp->apName == p_LD->apName))
                        {
                            PhysConnMap::iterator itPhyConn = pConAp->mapPhysConn.find(portList[i]);
                            if (itPhyConn != pConAp->mapPhysConn.end())
                            {
                                bFoundPort = true;
                            }
                        }
                    }
                }
                if (!bFoundPort)
                {
                    int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
                    string xml = pChecker->GetRowContentByOffset(p_In->offset);
                    string msg = "SCD�ļ�����������ߵ��ڲ���ַintAddr����˿�δ���������ҵ�:IED name=%s LDevice inst=%s Inputs �ڲ���ַintAddr=%s ����˿� Port=%s";
                    CInputsValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                        WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_In->inSignal).c_str(), WtoA(portList[i]).c_str());
                }
            }
        }

        // �ж������ź�
        wstring ldln = p_In->inSignal.substr(0, p_In->inSignal.find_first_of(L"."));
        wstring doda = p_In->inSignal.substr(p_In->inSignal.find_first_of(L".") + 1);
        wstring ln = ldln.substr(ldln.find_first_of(L"/") + 1);

        wstring doi = doda;
        wstring dai;
        if (doda.find_first_of(L".") != wstring::npos)
        {
            doi = doda.substr(0, doda.find_first_of(L"."));
            dai = doda.substr(doda.find_first_of(L".") + 1);
        }

        if(pConfigMgr->IsSignalValidSet())
        {
            wstring ldOfInsignal = p_In->inSignal.substr(0, p_In->inSignal.find_first_of(L"/"));
            wstring ldKey = p_LD->iedName + L"." + p_LD->apName + L"." + ldOfInsignal;
            IEDMap::iterator itRealIED = p_SCDCache->mapIED.find(p_LD->iedName);
            if (itRealIED != p_SCDCache->mapIED.end())
            {
                LDMap::iterator itRealLD = itRealIED->second.mapLD.find(ldKey);
                if (itRealLD != itRealIED->second.mapLD.end())
                {
                    LogicalNodeMap::iterator it = itRealLD->second.mapLogicalNode.find(ln);
                    if (it == itRealLD->second.mapLogicalNode.end())
                    {
                        int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
                        string xml = pChecker->GetRowContentByOffset(p_In->offset);
                        string msg = "SCD�ļ�����������ߵ��ڲ��̵�ַ������Ч����ʵ����δ�ҵ���ӦLnClass:IED name=%s LDevice inst=%s Inputs �ڲ���ַintAddr=%s";
                        CInputsValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                            WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_In->inSignal).c_str());
                    }
                    else
                    {
                        wstring lnType = it->second.lnType;
                        wstring type;
                        bool bToDo = false;
                        if (!CheckLNodeType(p_LD->iedName, lnType, doi, dai, type, bToDo))
                        {
                            int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
                            string xml = pChecker->GetRowContentByOffset(p_In->offset);
                            string msg = "SCD�ļ�����������ߵ��ڲ��̵�ַ������Ч����ģ����δ�ҵ�:IED name=%s LDevice inst=%s Inputs �ڲ���ַintAddr=%s";
                            CInputsValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                                WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_In->inSignal).c_str());
                        }
                    }
                }
            }
        }
        else
        {
            LogicalNodeMap::iterator it = p_LD->mapLogicalNode.find(ln);
            if (it == p_LD->mapLogicalNode.end())
            {
                int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
                string xml = pChecker->GetRowContentByOffset(p_In->offset);
                string msg = "SCD�ļ�����������ߵ��ڲ��̵�ַ������Ч����ʵ����δ�ҵ���ӦLnClass:IED name=%s LDevice inst=%s Inputs �ڲ���ַintAddr=%s";
                CInputsValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                    WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_In->inSignal).c_str());
            }
            else
            {
                wstring lnType = it->second.lnType;
                wstring type;
                bool bToDo = false;
                if (!CheckLNodeType(p_LD->iedName, lnType, doi, dai, type, bToDo))
                {
                    int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
                    string xml = pChecker->GetRowContentByOffset(p_In->offset);
                    string msg = "SCD�ļ�����������ߵ��ڲ��̵�ַ������Ч����ģ����δ�ҵ�:IED name=%s LDevice inst=%s Inputs �ڲ���ַintAddr=%s";
                    CInputsValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                        WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_In->inSignal).c_str());
                }
            }
        }

        // �ж�����ź�
        IEDMap::iterator itIED = p_SCDCache->mapIED.find(p_In->iedName);
        bool isOutExist = false;
        if (itIED != p_SCDCache->mapIED.end())
        {
            IEDCache *p_IED = &itIED->second;
            for (LDMap::iterator itLD = p_IED->mapLD.begin(); itLD != p_IED->mapLD.end(); itLD++)
            {
                // �˴�����LDΨһ
                LDCache *p_LD = &itLD->second;
                if (itLD->second.inst == p_In->ldInst)
                {
                    wstring datasetName;
                    for (ControlBlockInfoMap::iterator itGseCB = p_LD->mapGSEControl.begin(); itGseCB != p_LD->mapGSEControl.end(); itGseCB++)
                    {
                        DataSetMap::iterator itDS = p_LD->mapDataSet.find(itGseCB->second.dataSetName);
                        if (itDS != p_LD->mapDataSet.end())
                        {
                            DataSetCache *p_DS = &itDS->second;
                            datasetName = p_DS->name;
                            FCDAMap::iterator itFCDA = p_DS->mapFCDA.find(p_In->outSignal);
                            if (itFCDA != p_DS->mapFCDA.end())
                            {
                                isOutExist = true;
                                break;
                            }
                        }
                    }
                    for (ControlBlockInfoMap::iterator itSvCB = p_LD->mapSampledValueControl.begin(); itSvCB != p_LD->mapSampledValueControl.end(); itSvCB++)
                    {
                        // ����GOOSE�в��ҵ������˳�ѭ��
                        if (isOutExist)
                        {
                            break;
                        }

                        DataSetMap::iterator itDS = p_LD->mapDataSet.find(itSvCB->second.dataSetName);
                        if (itDS != p_LD->mapDataSet.end())
                        {
                            DataSetCache *p_DS = &itDS->second;
                            datasetName = p_DS->name;
                            FCDAMap::iterator itFCDA = p_DS->mapFCDA.find(p_In->outSignal);
                            if (itFCDA != p_DS->mapFCDA.end())
                            {
                                isOutExist = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
		if (!isOutExist)
		{
			int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
			string xml = pChecker->GetRowContentByOffset(p_In->offset);
			wstring outSignal = p_In->iedName + L"." + p_In->outSignal;
			string msg = "SCD�ļ�����������ߵ��ⲿ������Ч����GOOSE��SV���ݼ���δ�ҵ�:IED name=%s LDevice inst=%s Inputs �ⲿ����ExtRef=%s";
			CInputsValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
				WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(outSignal).c_str());
		}
    }
    return true;
}

// SCD�ļ�����������ߵĶ������������շ�˫���Ƿ�һ�»���ݣ�
bool CPreCheck::InputsDtValidCheck(LDCache *p_LD, InputsCache *p_In)
{
    if (CInputsDtDODAValidCheck::GetInstance()->IsEnabled() ||
        CInputsDtCDCValidCheck::GetInstance()->IsEnabled() ||
        CInputsDtbTypeValidCheck::GetInstance()->IsEnabled())
    {
        CConfigMgr *pConfigMgr = CConfigMgr::GetInstance();
        assert(pConfigMgr != NULL);

        wstring outType, inType;
        bool bOutSigToDo = false, bInSigToDo = false;

        // �ж������ź�
        {
            wstring lnInstance = p_In->inSignal.substr(p_In->inSignal.find_first_of(L"/") + 1);
            wstring ln = lnInstance.substr(0, lnInstance.find_first_of(L"."));
            wstring doda = lnInstance.substr(lnInstance.find_first_of(L".") + 1);

            wstring doi = doda;
            wstring dai;
            if (doda.find_first_of(L".") != wstring::npos)
            {
                doi = doda.substr(0, doda.find_first_of(L"."));
                dai = doda.substr(doda.find_first_of(L".") + 1);
            }

            // �ж������ź��Ƿ���ҪȥAP�µĲ���
            if (pConfigMgr->IsSignalValidSet())
            {
                wstring ldOfInsignal = p_In->inSignal.substr(0, p_In->inSignal.find_first_of(L"/"));
                wstring ldKey = p_LD->iedName + L"." + p_LD->apName + L"." + ldOfInsignal;
                IEDMap::iterator itRealIED = p_SCDCache->mapIED.find(p_LD->iedName);
                if (itRealIED != p_SCDCache->mapIED.end())
                {
                    LDMap::iterator itRealLD = itRealIED->second.mapLD.find(ldKey);
                    if (itRealLD != itRealIED->second.mapLD.end())
                    {
                        LogicalNodeMap::iterator itLN = itRealLD->second.mapLogicalNode.find(ln);
                        if (itLN != itRealLD->second.mapLogicalNode.end())
                        {
                            LogicalNodeCache *p_LN = &itLN->second;
                            CheckLNodeType(p_LD->iedName, p_LN->lnType, doi, dai, inType, bInSigToDo);
                        }
                        else
                        {
                            // δ�ҵ� ��ֱ�ӷ��أ���ֹ�ظ�����
                            // SCD�ļ�����������ߵ��ڲ��̵�ַ������Ч����ʵ����δ�ҵ���Ӧ
                            return false;
                        }
                    }
                }
            }
            else
            {
                LogicalNodeMap::iterator itLN = p_LD->mapLogicalNode.find(ln);
                if (itLN != p_LD->mapLogicalNode.end())
                {
                    LogicalNodeCache *p_LN = &itLN->second;
                    CheckLNodeType(p_LD->iedName, p_LN->lnType, doi, dai, inType, bInSigToDo);
                }
                else
                {
                    // δ�ҵ� ��ֱ�ӷ��أ���ֹ�ظ�����
                    // SCD�ļ�����������ߵ��ڲ��̵�ַ������Ч����ʵ����δ�ҵ���Ӧ
                    return false;
                }
            }
        }

        // �ж�����ź�
        IEDMap::iterator itIED = p_SCDCache->mapIED.find(p_In->iedName);
        if (itIED != p_SCDCache->mapIED.end())
        {
            IEDCache *p_IED = &itIED->second;
            for (LDMap::iterator itLD = p_IED->mapLD.begin(); itLD != p_IED->mapLD.end(); itLD++)
            {
                // �˴�����LDΨһ
                LDCache *p_LD = &itLD->second;
                if (p_LD->inst == p_In->ldInst)
                {
                    wstring lnInstance = p_In->outSignal.substr(p_In->outSignal.find_first_of(L"/") + 1);
                    wstring ln = lnInstance.substr(0, lnInstance.find_first_of(L"."));
                    wstring doda = lnInstance.substr(lnInstance.find_first_of(L".") + 1);

                    wstring doi = doda;
                    wstring dai;
                    if (doda.find_first_of(L".") != wstring::npos)
                    {
                        doi = doda.substr(0, doda.find_first_of(L"."));
                        dai = doda.substr(doda.find_first_of(L".") + 1);
                    }
                     
                    
                    LogicalNodeMap::iterator itLN = p_LD->mapLogicalNode.find(ln);
                    if (itLN != p_LD->mapLogicalNode.end())
                    {
                        LogicalNodeCache *p_LN = &itLN->second;
                        CheckLNodeType(p_LD->iedName, p_LN->lnType, doi, dai, outType, bOutSigToDo);
                    }
                    else
                    {
                        // δ�ҵ� ��ֱ�ӷ��أ���ֹ�ظ�����
                        // SCD�ļ�����������ߵ��ڲ��̵�ַ������Ч����ʵ����δ�ҵ���Ӧ
                        return false;
                    }
                }
            }
        }

        // DO/DA��DA/DO����ƥ��
        if (p_In->inSignal.size() > 0)
        {
            wstring insigDesc = L"";
            std::vector<std::wstring> inlist = Split(p_In->inSignal, L".");;
            if (bInSigToDo || inlist.size() == 2)
            {
                insigDesc = L"DO�Ͷ���";
            }
            else
            {
                insigDesc = L"DA�Ͷ���";
            }
            
            wstring outsigDesc = L"";
            std::vector<std::wstring> outlist = Split(p_In->outSignal, L".");;
            if (bOutSigToDo || outlist.size() == 2)
            {
                outsigDesc = L"DO�Ͷ���";
            }
            else
            {
                outsigDesc = L"DA�Ͷ���";
            }

            if (outsigDesc != insigDesc)
            {
                int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
                string xml = pChecker->GetRowContentByOffset(p_In->offset);
                string msg = "SCD�ļ�����������ߵĶ������������շ�˫���ź����Ͳ�ƥ��:IED name=%s LDevice inst=%s Inputs �ڲ���ַintAddr=%s �ڲ���ַintAddr����=%s �ⲿ����ExtRef=%s �ⲿ����ExtRef����=%s";
                CInputsDtDODAValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                    WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_In->inSignal).c_str(), WtoA(insigDesc).c_str(), WtoA(p_In->iedName + L"." + p_In->outSignal).c_str(), WtoA(outsigDesc).c_str());
            }
        }
        // DO/DO CDC��һ��
        if (bInSigToDo == true &&
            bOutSigToDo == true &&
            inType != outType)
        {
            int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
            string xml = pChecker->GetRowContentByOffset(p_In->offset);
            string msg = "SCD�ļ�����������ߵĶ�����������DO/DO CDC��һ��:IED name=%s LDevice inst=%s Inputs �ڲ���ַintAddr=%s �ڲ���ַintAddr����=%s �ⲿ����ExtRef=%s �ⲿ����ExtRef����=%s";
            CInputsDtCDCValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_In->inSignal).c_str(), WtoA(inType).c_str(), WtoA(p_In->iedName + L"." + p_In->outSignal).c_str(), WtoA(outType).c_str());
        }

        // ����SCD�ļ�У���У����������bType����һ�£�DA����У����Ҫ�޸Ĺ���
        // ����INT���ͣ���INT8��INT16��INT24��INT32��INT128������Ϊ��һ�µģ�
        // ����INTU���ͣ���INT8U��INT16U��INT24U��INT32U������Ϊ��һ�µģ�
        // ����FLOAT���ͣ���FLOAT32��FLOAT64������Ϊ��һ�µģ�
        // ����VisString���ͣ���VisString32��64��255����Ϊ��һ�µģ�
        wstring newInType, newOutType;
        for (int iCount = 0; iCount < inType.size(); iCount++)
        {
            if (inType[iCount] < L'0' || inType[iCount] > L'9')
            {
                newInType.push_back(inType[iCount]);
            }
        }
        for (int iCount = 0; iCount < outType.size(); iCount++)
        {
            if (outType[iCount] < L'0' || outType[iCount] > L'9')
            {
                newOutType.push_back(outType[iCount]);
            }
        }
        
        // DA/DA bType��һ�£�
        if (bInSigToDo == false &&
            bOutSigToDo == false &&
            newInType != newOutType)
        {
            int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
            string xml = pChecker->GetRowContentByOffset(p_In->offset);
            string msg = "SCD�ļ�����������ߵĶ�����������DA/DA bType��һ��:IED name=%s LDevice inst=%s Inputs �ڲ���ַintAddr=%s �ڲ���ַintAddr����=%s �ⲿ����ExtRef=%s �ⲿ����ExtRef����=%s";
            CInputsDtbTypeValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_In->inSignal).c_str(), WtoA(inType).c_str(), WtoA(p_In->iedName + L"." + p_In->outSignal).c_str(), WtoA(outType).c_str());
        }
    }
    return true;
}

bool CPreCheck::InputsDescValidCheck(LDCache *p_LD, InputsCache *p_In)
{
    if (CInputsDescValid1Check::GetInstance()->IsEnabled())
    {
        wstring intSigDesc, outSigDesc;

        // �ж������ź�
        LogicalNodesInstancesMap::iterator itLN = p_LD->mapSignalInstance.find(p_In->inSignal);
        if (itLN != p_LD->mapSignalInstance.end())
        {
            LogicalNodesInstanceCache *p_LN = &itLN->second;
            if (p_LN->dUVal.size() > 0)
            {
                intSigDesc = p_LN->dUVal;
            }else
            {
                intSigDesc = p_LN->DOIDesc;
            }
            
        }

        // �ж�����ź�
        IEDMap::iterator itIED = p_SCDCache->mapIED.find(p_In->iedName);
        if (itIED != p_SCDCache->mapIED.end())
        {
            IEDCache *p_IED = &itIED->second;
            for (LDMap::iterator itLD = p_IED->mapLD.begin(); itLD != p_IED->mapLD.end(); itLD++)
            {
                // �˴�����LDΨһ
                LDCache *p_LD = &itLD->second;
                if (itLD->first.find(p_In->ldInst) != wstring::npos)
                {
                    LogicalNodesInstancesMap::iterator itLN = p_LD->mapSignalInstance.find(p_In->outSignal);
                    if (itLN != p_LD->mapSignalInstance.end())
                    {
                        LogicalNodesInstanceCache *p_LN = &itLN->second;
                        if (p_LN->dUVal.size() > 0)
                        {
                            outSigDesc = p_LN->dUVal;
                        }else
                        {
                            outSigDesc = p_LN->DOIDesc;
                        }
                        break;
                    }
                }
            }
        }
        InputsDescValid1Check(intSigDesc, outSigDesc, p_LD, p_In);
        InputsDescValid2Check(intSigDesc, outSigDesc, p_LD, p_In);
    }
    return true;
}

// �������֣���բ�ź���������ʧ�飬����ʧ���ź�������բ
bool CPreCheck::InputsDescValid1Check( wstring &inSigDesc, wstring &outSigDesc, LDCache *p_LD, InputsCache *p_In)
{
    if (CInputsDescValid1Check::GetInstance()->IsEnabled())
    {
        if (outSigDesc.find(L"��") != wstring::npos)
        {
            if (inSigDesc.find(L"����ʧ��") != wstring::npos)
            {
                int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
                string xml = pChecker->GetRowContentByOffset(p_In->offset);
                wstring outsig = p_In->iedName + L"." + p_In->ldInst + L"/" + p_In->outSignal;
                string msg = "��ȷ����բ�źŲ�����������ʧ�飬����ʧ���źŲ���������բ�ź�:IED name=%s LDevice inst=%s Inputs �ڲ���ַintAddr=%s inDesc=%s outSignale=%s outDesc=%s";
                CInputsDescValid1Check::GetInstance()->Report(row, xml, msg.c_str(), 
                    WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_In->inSignal).c_str(), WtoA(inSigDesc).c_str(), WtoA(outsig).c_str(), WtoA(outSigDesc).c_str());
                return true;
            }
        }
    }
    return true;
}

// ������λ���Ӵ�����բ����ʧ����λ���Ӵ���
bool CPreCheck::InputsDescValid2Check( wstring &inSigDesc, wstring &outSigDesc, LDCache *p_LD, InputsCache *p_In)
{
    if (CInputsDescValid2Check::GetInstance()->IsEnabled())
    {
        bool isExist = false;

        // ת����Сд����
        transform(outSigDesc.begin(), outSigDesc.end(), outSigDesc.begin(), towupper);
        transform(inSigDesc.begin(), inSigDesc.end(), inSigDesc.begin(), towupper);
        // ��λ����
        if (outSigDesc.find(L"A��") != wstring::npos)
        {
            if (inSigDesc.find(L"B��") != wstring::npos || 
                inSigDesc.find(L"C��") != wstring::npos)
            {
                isExist = true;
            }
        }
        else if (outSigDesc.find(L"B��") != wstring::npos)
        {
            if (inSigDesc.find(L"A��") != wstring::npos || 
                inSigDesc.find(L"C��") != wstring::npos)
            {
                isExist = true;
            }
        } 
        else if (outSigDesc.find(L"C��") != wstring::npos)
        {
            if (inSigDesc.find(L"A��") != wstring::npos || 
                inSigDesc.find(L"B��") != wstring::npos)
            {
                isExist = true;
            }
        }
        // �������
        if (outSigDesc.find(L"AB��") != wstring::npos)
        {
            if (inSigDesc.find(L"BC��") != wstring::npos || 
                inSigDesc.find(L"CA��") != wstring::npos)
            {
                isExist = true;
            }
        }
        else if (outSigDesc.find(L"BC��") != wstring::npos)
        {
            if (inSigDesc.find(L"AB��") != wstring::npos || 
                inSigDesc.find(L"CA��") != wstring::npos)
            {
                isExist = true;
            }
        } 
        else if (outSigDesc.find(L"CA��") != wstring::npos)
        {
            if (inSigDesc.find(L"AB��") != wstring::npos || 
                inSigDesc.find(L"BC��") != wstring::npos)
            {
                isExist = true;
            }
        }

        if (isExist)
        {
            int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
            string xml = pChecker->GetRowContentByOffset(p_In->offset);
            string msg = "���������źŶ�Ӧ����λӦ��һ��:IED name=%s LDevice inst=%s Inputs �ڲ���ַintAddr=%s inDesc=%s outSignale=%s outDesc=%s";
            CInputsDescValid1Check::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_In->inSignal).c_str(), 
                WtoA(inSigDesc).c_str(), WtoA(p_In->outSignal).c_str(), WtoA(outSigDesc).c_str());
        }
    }

    return true;
}

// ������ݼ��ж����du��Do�����Ƿ�һ��  change by zxc 151126
// �߼���������ȡ���ݼ���FCDAʵ��������Ƿ�ʵ��������δ��ʵ����������ʾ������
//           �ڼ��FCDA�Ƿ�DO��������������������
//           ����DO����Ϊ�գ�����ʾ������
//           ����DO�����du������Ƿ����dUʵ����������������ʾ������
//           �ݱȽ�Do������dUֵ�Ƿ�һ�£���һ������ʾ

bool CPreCheck::DoiInstanceCheck()
{
    if (!CDoiDescBlankCheck::GetInstance()->IsEnabled())
    {
        return true;
    }
    if (NULL != p_SCDCache)
    {
        for (IEDMap::iterator itIED = p_SCDCache->mapIED.begin(); itIED != p_SCDCache->mapIED.end(); itIED++)
        {
            IEDCache *p_IED = &itIED->second;
            for (LDMap::iterator itLD = p_IED->mapLD.begin(); itLD != p_IED->mapLD.end(); itLD++)
            {
                LDCache *p_LD = &itLD->second;
                //for (LogicalNodesInstancesMap::iterator itLN = p_LD->mapSignalInstance.begin(); itLN != p_LD->mapSignalInstance.end(); itLN++)
                //{
                //    LogicalNodesInstanceCache *p_LNInstance = &itLN->second;
                //    DoiDescBlackCheck(p_LD, p_LNInstance);
                //}
                for (DataSetMap::iterator itDataSet = p_LD->mapDataSet.begin(); itDataSet != p_LD->mapDataSet.end(); itDataSet++)
                {
                    DataSetCache *p_DataSet = &itDataSet->second;
                    for (FCDAMap::iterator itFCDA = p_DataSet->mapFCDA.begin(); itFCDA != p_DataSet->mapFCDA.end(); itFCDA++)
                    {
                        FCDACache *p_FCDA = &itFCDA->second;
                        wstring lnKey = p_FCDA->key;
                        std::vector<wstring> strList = Split(p_FCDA->key, L".");
                        if (strList.size() > 2 && p_FCDA->daName != L"")
                        {
                            lnKey = lnKey.substr(0, lnKey.find_last_of(L'.'));
                        }
                        LogicalNodesInstancesMap::iterator itLN = p_LD->mapSignalInstance.find(lnKey);
                        if (itLN != p_LD->mapSignalInstance.end())
                        {
                            LogicalNodesInstanceCache *p_LNInstance = &itLN->second;
                            DoiDescBlackCheck(p_LD, p_LNInstance, p_FCDA);
                        }
                        else
                        {
                            int row = pChecker->CalcRowNumberFromOffset(p_FCDA->offset);
                            string xml = pChecker->GetRowContentByOffset(p_FCDA->offset);
                            string msg = "���ݼ�Ԫ��δ��LN���ҵ�:IED name=%s LDevice inst=%s DataSet=%s FCDA=%s";
                            CDoiDescBlankCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                                WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_DataSet->name).c_str(), WtoA(p_FCDA->key).c_str());
                        }
                        
                    }
                    
                }
            }
        }
    }
    return true;
}

// ʵ����DOIԪ�صġ�desc������ֵ�롰dU�� ����ֵ�Ƿ�ǿ�������һ�£�
bool CPreCheck::DoiDescBlackCheck(LDCache *p_LD, LogicalNodesInstanceCache *p_LNInstance, FCDACache *p_FCDA)
{
    if (CDoiDescBlankCheck::GetInstance()->IsEnabled())
    {

        std::vector<wstring> keyList = Split(p_LNInstance->key, L".");
        //if (keyList.size() == 2)
        //{
            wstring doiDesc = p_LNInstance->DOIDesc;
            if (doiDesc.size() == 0)
            {
                int row = pChecker->CalcRowNumberFromOffset(p_FCDA->offset);
                string xml = pChecker->GetRowContentByOffset(p_FCDA->offset);
                string msg = "ʵ����DOIԪ�صġ�desc������ֵΪ��:IED name=%s LDevice inst=%s FCDA=%s";
                CDoiDescBlankCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                    WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_FCDA->key).c_str());
                return false;
            }

            wstring duKey = p_LNInstance->key + L".dU";
            LogicalNodesInstancesMap::iterator itDu = p_LD->mapSignalInstance.find(duKey);
            if (itDu != p_LD->mapSignalInstance.end())
            {
                if (doiDesc != itDu->second.dUVal)
                {
                    int row = pChecker->CalcRowNumberFromOffset(p_FCDA->offset);
                    string xml = pChecker->GetRowContentByOffset(p_FCDA->offset);
                    string msg = "ʵ����DOIԪ�صġ�desc������ֵ�롰dU������ֵ��һ��:IED name=%s LDevice inst=%s FCDA=%s";
                    CDoiDescBlankCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                        WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_FCDA->key).c_str());
                    return false;
                }
            }
            else
            {
                int row = pChecker->CalcRowNumberFromOffset(p_FCDA->offset);
                string xml = pChecker->GetRowContentByOffset(p_FCDA->offset);
                string msg = "Do��û��dU��֮��Ӧ:IED name=%s LDevice inst=%s FCDA=%s";
                CDoiDescBlankCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                    WtoA(p_LD->iedName).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_FCDA->key).c_str());
                return false;
            }
        // }
    }
    return true;
}

// ʵ��������DOI/SDI/DAI�Ķ��������Ƿ�����������ģ�嶨������ݶ���ṹһ��
bool CPreCheck::DaTempValidCheck()
{
    if (CDaTempValidCheck::GetInstance()->IsEnabled())
    {
        bool bToDo = false;
        CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
        SCDCache*p_SCDCache = &pCacheMgr->m_SCDCache;
        if (p_SCDCache != NULL)
        {
            for (IEDMap::iterator itIED = p_SCDCache->mapIED.begin(); itIED != p_SCDCache->mapIED.end(); itIED++)
            {
                IEDCache *pIED = &itIED->second;
                for (LDMap::iterator itLD = pIED->mapLD.begin(); itLD != pIED->mapLD.end(); itLD++)
                {
                    LDCache *pLD = &itLD->second;
                    for (LogicalNodesInstancesMap::iterator itLI = pLD->mapSignalInstance.begin(); itLI != pLD->mapSignalInstance.end(); itLI++)
                    {
                        LogicalNodesInstanceCache *pLI = &itLI->second;
                        if ((m_LogicalNodeInstance != L"") && (pLI->key.find(m_LogicalNodeInstance) != wstring::npos))
                        {
                            continue;
                        }
                        wstring type;
                        CheckLNodeType(pLD->iedName, pLI, type, bToDo);
                    }
                }
            }
        }
    }
    return true;
}

// ����LnType����
// �˲��ֵ�typeֻ�����ж����·���˵��ź������Ƿ�һ�£���
bool CPreCheck::CheckLNodeType(wstring IEDName, LogicalNodesInstanceCache *pLI, wstring &type, bool &bToDo)
{
    bool isFound = false;

    // ����ʵ���е�lnType��������ģ����LNodeType��
    SCDCache *p_SCDCache = &CCacheMgr::GetInstance()->m_SCDCache;

    LNodeTypeMap::iterator it = p_SCDCache->sDTTCache.mapLNode.find(pLI->LNType);
    if (it != p_SCDCache->sDTTCache.mapLNode.end())
    {
        LNodeTypeCache *pLnType = &it->second;
        // pLnType->bUnUsed = false; // ��¼���ѱ�ʹ�ã�����DataTempRedunCheck

        DOMap::iterator itLDO = pLnType->mapDO.find(pLI->DOI);
        if (itLDO != pLnType->mapDO.end())
        {
            DOCache *itDo = &itLDO->second;
            // itDo->bUnUsed = false;
            if (CheckDOType(itDo->type, pLI->DAI, type, bToDo))
            {
                isFound = true;
            }
        }
    } 

    if (CDaTempValidCheck::GetInstance()->IsEnabled() && !isFound)
    {
        m_LogicalNodeInstance = pLI->key;
        int row = pChecker->CalcRowNumberFromOffset(pLI->offset);
        string xml = pChecker->GetRowContentByOffset(pLI->offset);
        string msg = "ʵ�������ö�����������������ģ�嶨������ݶ���ṹ��һ��:IED name=%s �������� %s";
        CDaTempValidCheck::GetInstance()->Report(row, xml, msg.c_str(), WtoA(IEDName).c_str(), WtoA(pLI->key).c_str());
    }

    return true;
}

bool CPreCheck::CheckLNodeType(wstring IEDName, wstring LNType, wstring DOI, wstring DAI, wstring &type, bool &bToDo)
{
    bool isFound = false;

    // ����ʵ���е�lnType��������ģ����LNodeType��
    SCDCache *p_SCDCache = &CCacheMgr::GetInstance()->m_SCDCache;

    LNodeTypeMap::iterator it = p_SCDCache->sDTTCache.mapLNode.find(LNType);
    if (it != p_SCDCache->sDTTCache.mapLNode.end())
    {
        LNodeTypeCache *pLnType = &it->second;
        // pLnType->bUnUsed = false; // ��¼���ѱ�ʹ�ã�����DataTempRedunCheck

        std::vector<wstring> list = Split(DOI, L".");
        wstring dai;
        if (list.size() > 1)
        {
            dai = DOI.substr(DOI.find_first_of(L".") + 1);
            if (DAI.size() > 0)
            {
                dai = dai + L"." + DAI;
            }
        }
        else
        {
            dai = DAI;
        }

        DOMap::iterator itLDO = pLnType->mapDO.find(list[0]);
        if (itLDO != pLnType->mapDO.end())
        {
            DOCache *itDo = &itLDO->second;
            // itDo->bUnUsed = false;
            if (CheckDOType(itDo->type, dai, type, bToDo))
            {
                isFound = true;
            }
        }
    } 
    return isFound;
}

// ʵ��������DOI/SDI/DAI�Ķ��������Ƿ�����������ģ�嶨������ݶ���ṹһ��
// ����DOType����
bool CPreCheck::CheckDOType(wstring &doType, wstring &key, wstring &type, bool &bToDo)
{
    SCDCache *p_SCDCache = &CCacheMgr::GetInstance()->m_SCDCache;
    DOTypeMap::iterator itDO = p_SCDCache->sDTTCache.mapDoType.find(doType);
    if (itDO != p_SCDCache->sDTTCache.mapDoType.end())
    {
        DOTypeCache *pDO = &itDO->second;
        // pDO->bUnUsed = false;

        // ���ʵ����DO,������DA���򷵻�true
        if (key.size() == 0)
        {
            type = pDO->cdc;
            bToDo = true;
            return true;
        }
        // ������в��
        std::vector<wstring> list = Split(key, L".");
        if (list.size() > 0)
        {
            DOMap::iterator itSDO = pDO->mapSDO.find(list[0]);
            if (itSDO != pDO->mapSDO.end())
            {
                // itSDO->second.bUnUsed = false;
                wstring newKey = L"";
                if (list.size() > 1)
                {
                    newKey = key.substr(key.find_first_of(L".")+1);
                }
                
                if (CheckDOType(itSDO->second.type, newKey, type, bToDo))
                {
                    return true;
                }
            }

            DAMap::iterator itDA = pDO->mapDaType.find(list[0]);
            if (itDA != pDO->mapDaType.end())
            {
                if (1 == list.size())
                {
                    type = itDA->second.bType;
                    // itDA->second.bUnUsed = true;
                    bToDo = false;
                    return true;
                }
                else
                {
                    wstring datype = itDA->second.type;
                    wstring newKey = key.substr(key.find_first_of(L".")+1);
                    if (CheckDAType(datype, newKey, type, bToDo))
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool CPreCheck::CheckDAType(wstring &daType, wstring &key, wstring &type, bool &bToDo)
{
    SCDCache *p_SCDCache = &CCacheMgr::GetInstance()->m_SCDCache;
    DATypeMap::iterator itDAType = p_SCDCache->sDTTCache.mapDaType.find(daType);
    if (itDAType != p_SCDCache->sDTTCache.mapDaType.end())
    {
        DATypeCache *pDA = &itDAType->second;
        // pDA->bUnUsed = false;

        std::vector<wstring> list = Split(key, L".");
        if (list.size() > 0)
        {
            BDAMap::iterator itBDA = pDA->mapBDA.find(list[0]);
            if (itBDA != pDA->mapBDA.end())
            {
                // itBDA->second.bUnUsed = false;
                if (1 == list.size())
                {
                    type = itBDA->second.bType;
                    bToDo = false;
                    return true;
                }
                else
                {
                    wstring newKey = key.substr(key.find_first_of(L".")+1);
                    if (CheckDAType(itBDA->second.type, newKey, type, bToDo))
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// ����δ�����õ�LNodeType��DOType��DAType
bool CPreCheck::DataTempRedunCheck()
{
    if (CDataTempRedunCheck::GetInstance()->IsEnabled())
    {
        // �����
        SCDCache*p_SCDCache = &CCacheMgr::GetInstance()->m_SCDCache;

        std::set<wstring> UsedType;
        if (p_SCDCache != NULL)
        {
            // ���汻ʹ�õ�����
            for (IEDMap::iterator itIED = p_SCDCache->mapIED.begin(); itIED != p_SCDCache->mapIED.end(); itIED++)
            {
                IEDCache *pIED = &itIED->second;
                for (LDMap::iterator itLD = pIED->mapLD.begin(); itLD != pIED->mapLD.end(); itLD++)
                {
                    LDCache *pLD = &itLD->second;
                    for (LogicalNodeMap::iterator itLN = pLD->mapLogicalNode.begin(); itLN != pLD->mapLogicalNode.end(); itLN++)
                    {
                        LogicalNodeCache *pLN = &itLN->second;
                        UsedType.insert(pLN->lnType);
                    }
                }
            }

            for (LNodeTypeMap::iterator itLN = p_SCDCache->sDTTCache.mapLNode.begin(); itLN != p_SCDCache->sDTTCache.mapLNode.end(); itLN++)
            {
                LNodeTypeCache *p_LNType = &itLN->second;
                for (DOMap::iterator itDO = p_LNType->mapDO.begin(); itDO != p_LNType->mapDO.end(); itDO++)
                {
                    UsedType.insert(itDO->second.type);
                }
            }

            for (DOTypeMap::iterator itDO = p_SCDCache->sDTTCache.mapDoType.begin(); itDO != p_SCDCache->sDTTCache.mapDoType.end(); itDO ++)
            {
                DOTypeCache *pDO = &itDO->second;
                for (DAMap::iterator itDA = pDO->mapDaType.begin(); itDA != pDO->mapDaType.end(); itDA++)
                {
                    UsedType.insert(itDA->second.type);
                }

                for (DOMap::iterator itSDO = pDO->mapSDO.begin(); itSDO != pDO->mapSDO.end(); itSDO++)
                {
                    UsedType.insert(itSDO->second.type);
                }
            }
            
            for (DATypeMap::iterator itDA = p_SCDCache->sDTTCache.mapDaType.begin(); itDA != p_SCDCache->sDTTCache.mapDaType.end(); itDA++)
            {
                DATypeCache *p_DA = &itDA->second;
                for (BDAMap::iterator itBDA = p_DA->mapBDA.begin(); itBDA != p_DA->mapBDA.end(); itBDA++)
                {
                    UsedType.insert(itBDA->second.type);
                }
            }

            // �ж��Ƿ�ʹ��
            for (LNodeTypeMap::iterator itLN = p_SCDCache->sDTTCache.mapLNode.begin(); itLN != p_SCDCache->sDTTCache.mapLNode.end(); itLN++)
            {
                LNodeTypeCache *p_LN = &itLN->second;
                if (UsedType.count(p_LN->id) == 0)
                {
                    int row = pChecker->CalcRowNumberFromOffset(p_LN->offset);
                    string xml = pChecker->GetRowContentByOffset(p_LN->offset);
                    string msg = "����δ�����õ�LNodeType:LNodeType id=%s";
                    CDataTempRedunCheck::GetInstance()->Report(row, xml, msg.c_str(), WtoA(p_LN->id).c_str());
                }
            }

            for (DOTypeMap::iterator itDO = p_SCDCache->sDTTCache.mapDoType.begin(); itDO != p_SCDCache->sDTTCache.mapDoType.end(); itDO ++)
            {
                DOTypeCache *pDO = &itDO->second;
                if (UsedType.count(pDO->id) == 0)
                {
                    int row = pChecker->CalcRowNumberFromOffset(pDO->offset);
                    string xml = pChecker->GetRowContentByOffset(pDO->offset);
                    string msg = "����δ�����õ�DOType:DOType id=%s";
                    CDataTempRedunCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                        WtoA(pDO->id).c_str());
                }
            }

            for (DATypeMap::iterator itDA = p_SCDCache->sDTTCache.mapDaType.begin(); itDA != p_SCDCache->sDTTCache.mapDaType.end(); itDA++)
            {
                DATypeCache *p_DA = &itDA->second;
                if (UsedType.count(p_DA->id) == 0)
                {
                    int row = pChecker->CalcRowNumberFromOffset(p_DA->offset);
                    string xml = pChecker->GetRowContentByOffset(p_DA->offset);
                    string msg = "����δ�����õ�DAType:DAType id=%s";
                    CDataTempRedunCheck::GetInstance()->Report(row, xml, msg.c_str(), WtoA(p_DA->id).c_str());
                }
            }
            for (EnumTypeCacheMap::iterator itEnum = p_SCDCache->sDTTCache.mapEnumType.begin(); itEnum != p_SCDCache->sDTTCache.mapEnumType.end(); itEnum++)
            {
                EnumTypeCache *p_En = &itEnum->second;
                if (UsedType.count(p_En->id) == 0)
                {
                    int row = pChecker->CalcRowNumberFromOffset(p_En->offset);
                    string xml = pChecker->GetRowContentByOffset(p_En->offset);
                    string msg = "����δ�����õ�EnumType:EnumType id=%s";
                    CDataTempRedunCheck::GetInstance()->Report(row, xml, msg.c_str(), WtoA(p_En->id).c_str());
                }
            }

        }
    }
    return true;
}

// GOOSE ��SV���õ�ExtRef�ڲ�intAddr��ֵ�Ƿ���Ϲ淶�е�������ʽҪ�󣬹淶��ʽӦΪ��LD/LN.DO.DA��(Q/GDW 396-2009)��n-A:LD/LN.DO.DA����Q/GDW 396-2012����
bool CPreCheck::InputsFormatCheck(LDCache &ldCache, InputsCache *p_In)
{
    bool bFormatError = false;
    // �̵�ַΪ��
    if (p_In->intAddr.size() == 0)
    {
        bFormatError = true;
    }
    // ����˿�Ϊ��
    int pos = p_In->intAddr.find(L":");
    if (pos != wstring::npos && pos == 0)
    {
        bFormatError = true;
    }
    // ��ֻ��LNҲΪ��
    if (p_In->intAddr.find(L".") == wstring::npos)
    {
        bFormatError = true;
    }
    // �������Ƿ��ַ�
    string temp = WtoA(p_In->intAddr);
    for (int i = 0; i < temp.size(); i++)
    {
        char c = temp.at(i);
        // �ַ���"A-Z"��"a-z"��"��"��"/"����-������_����
        if ((c < '-') || ((c > ':') && (c < 'A')) || ((c > 'Z') && (c < '_')) || ((c > '_') && (c < 'a')) || (c > 'z'))
        {
            bFormatError = true;
            break;
        }
    }

    if (bFormatError)
    {
        int row = pChecker->CalcRowNumberFromOffset(p_In->offset);
        string xml = pChecker->GetRowContentByOffset(p_In->offset);
        string msg = "ExtRef�ڲ�intAddr��ֵ�����Ƿ��ַ�:IED name=%s LDevice inst=%s �ڲ���ַintAddr=%s";
        if (CInputsValidCheck::GetInstance()->IsEnabled())
        {
            CInputsValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(ldCache.iedName).c_str(),
                WtoA(ldCache.inst).c_str(),
                WtoA(p_In->intAddr).c_str());
        }
        else if (CInputsDtDODAValidCheck::GetInstance()->IsEnabled())
        {
            CInputsValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(ldCache.iedName).c_str(),
                WtoA(ldCache.inst).c_str(),
                WtoA(p_In->intAddr).c_str());
        }
        else if (CInputsDtCDCValidCheck::GetInstance()->IsEnabled())
        {
            CInputsValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(ldCache.iedName).c_str(),
                WtoA(ldCache.inst).c_str(),
                WtoA(p_In->intAddr).c_str());
        }
        else if (CInputsDtbTypeValidCheck::GetInstance()->IsEnabled())
        {
            CInputsValidCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(ldCache.iedName).c_str(),
                WtoA(ldCache.inst).c_str(),
                WtoA(p_In->intAddr).c_str());
        }
        else if (CInputsDescValid1Check::GetInstance()->IsEnabled())
        {
            CInputsDescValid1Check::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(ldCache.iedName).c_str(),
                WtoA(ldCache.inst).c_str(),
                WtoA(p_In->intAddr).c_str());
        }
        else if (CInputsDescValid2Check::GetInstance()->IsEnabled())
        {
            CInputsDescValid2Check::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(ldCache.iedName).c_str(),
                WtoA(ldCache.inst).c_str(),
                WtoA(p_In->intAddr).c_str());
        }
    }
    return bFormatError;
}

