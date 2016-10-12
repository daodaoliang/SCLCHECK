#include "CacheMgr.h"
#include "ScdChecker.h"
#include "../ScdCheckCommon/charconv.h"
#include <string>

#include "CheckItems/CommunicationGSEorSVUniqueCheck.h"
#include "CheckItems/IpUniqueCheck.h"
#include "CheckItems/InputsDescValid2Check.h"
#include "CheckItems/ControlIdUniqueCheck.h"
#include "CheckItems/InputsFormatCheck.h"
#include "CheckItems/CommunicationRedunCheck.h"
#include "CheckItems/GseMacValidCheck.h"
#include "CheckItems/InputsDescValid1Check.h"
#include "CheckItems/IpValidCheck.h"
#include "CheckItems/LDescBlankCheck.h"
#include "CheckItems/GseAppidValidCheck.h"
#include "CheckItems/SmvAppidValidCheck.h"
#include "CheckItems/DaTempValidCheck.h"
#include "CheckItems/FcdaValidCheck.h"
#include "CheckItems/DataTempRedunCheck.h"
#include "CheckItems/XmlCheck.h"
#include "CheckItems/DoiDescBlankCheck.h"
#include "CheckItems/SubFormatCheck.h"
#include "CheckItems/SchemaCheck.h"
#include "CheckItems/CrcBlankCheck.h"
#include "CheckItems/AppidUniqueCheck.h"
#include "CheckItems/InputsStadValidCheck.h"
#include "CheckItems/VlanValidCheck.h"
#include "CheckItems/SmvMacValidCheck.h"
#include "CheckItems/BayValid2Check.h"
#include "CheckItems/IntAddrUniqueCheck.h"
#include "CheckItems/MacAddrUniqueCheck.h"
#include "CheckItems/_PreCheck.h"
#include "CheckItems/GeneralCheck.h"
#include "../ScdCheckCommon/deelx.h"

#include "CheckItems/SSDSubstationCheck.h"
#include "CheckItems/SSDVoltageLevelCheck.h"
#include "CheckItems/SSDBayCheck.h"
#include "CheckItems/SSDPowerTransformerCheck.h"
#include "CheckItems/SSDConductingEquipmentCheck.h"
#include "CheckItems/SSDConnectivityNodeCheck.h"
#include "CheckItems/SSDTerminalCheck.h"
#include "CheckItems/SSDLNodeCheck.h"
#include "CheckItems/SSDCommonEquipmentCheck.h"

#include "CheckItems/SCDHeaderCheck.h"
#include "CheckItems/SCDCommunicationCheck.h"
#include "CheckItems/SCDSubNetWorkCheck.h"
#include "CheckItems/SCDConnectedAPCheck.h"
#include "CheckItems/SCDIPAddressCheck.h"
#include "CheckItems/SCDGSECheck.h"
#include "CheckItems/SCDSMVCheck.h"
#include "CheckItems/SCDPhysConnCheck.h"

#include "CheckItems/SCDIEDCheck.h"
#include "CheckItems/SCDIEDAccessPointCheck.h"
#include "CheckItems/SCDIEDServicesCheck.h"
#include "CheckItems/SCDIEDLNCheck.h"

#include "CheckItems/SCDLNodeTypeCheck.h"
#include "CheckItems/SCDDOTypeCheck.h"
#include "CheckItems/SCDDATypeCheck.h"
#include "CheckItems/SCDEnumTypeCheck.h"
// -----------------------------------------SSD������------------------------------------------
#include "CheckItems/SSDSubstationCheck_Unique.h"
#include "CheckItems/SSDSubstationCheck_Normative.h"
#include "CheckItems/SSDSubstationCheck_Other.h"
#include "CheckItems/SSDVoltageLevelCheck_Unique.h"
#include "CheckItems/SSDVoltageLevelCheck_UnNULL.h"
#include "CheckItems/SSDVoltageLevelCheck_Normative.h"
#include "CheckItems/SSDVoltageLevelCheck_Other.h"
#include "CheckItems/SSDVoltageLevelCheck_Sort.h"

#include "CheckItems/SSDBayCheck_AttrNormative.h"
#include "CheckItems/SSDBayCheck_dTypeNormative.h"
#include "CheckItems/SSDBayCheck_LineBay.h"
#include "CheckItems/SSDPowerTransformerCheck_Connection.h"
#include "CheckItems/SSDPowerTransformerCheck_Unique.h"
#include "CheckItems/SSDPowerTransformerCheck_AttrNormative.h"
#include "CheckItems/SSDPowerTransformerCheck_WindingNormative.h"
#include "CheckItems/SSDTerminalCheck_NameNormative.h"
#include "CheckItems/SSDTerminalCheck_CNodeConnection.h"
#include "CheckItems/SSDTerminalCheck_Unique.h"
#include "CheckItems/SSDTerminalCheck_Valid.h"
#include "CheckItems/SSDConnectivityNodeCheck_PathUnique.h"
#include "CheckItems/SSDConductingEquipmentCheck_IFLEquipment.h"
#include "CheckItems/SSDConnectivityNodeCheck_NameNormative.h"
#include "CheckItems/SSDConductingEquipmentCheck_Unique.h"
#include "CheckItems/SSDConnectivityNodeCheck_UnUsed.h"

#include "Winsock2.h"

typedef wstring::size_type (wstring::*find_t)(const wstring& delim, 
    wstring::size_type offset) const;
vector<wstring> Split(const wstring& s,
    const wstring& match,
    bool removeEmpty,
    bool fullMatch)
{
    vector<wstring> result;                 // return container for tokens
    wstring::size_type start = 0,           // starting position for searches
        skip = 1;            // positions to skip after a match
    find_t pfind = &wstring::find_first_of; // search algorithm for matches

    if (fullMatch)
    {
        // use the whole match string as a key
        // instead of individual characters
        // skip might be 0. see search loop comments
        skip = match.length();
        pfind = &wstring::find;
    }

    while (start != wstring::npos)
    {
        // get a complete range [start..end)
        wstring::size_type end = (s.*pfind)(match, start);

        // null strings always match in string::find, but
        // a skip of 0 causes infinite loops. pretend that
        // no tokens were found and extract the whole string
        if (skip == 0) end = wstring::npos;

        wstring token = s.substr(start, end - start);

        if (!(removeEmpty && token.empty()))
        {
            // extract the token and add it to the result list
            result.push_back(token);
        }

        // start the next range
        if ((start = end) != wstring::npos) start += skip;
    }

    return result;
}

CCacheMgr::CCacheMgr(void) : m_vn(NULL)
{
    m_DueSCDElementInfo = 0.0;
    m_LoseSCDElementInfo = 0.0;
}

CCacheMgr::~CCacheMgr(void)
{
}

void CCacheMgr::AddNeededCacheMark(const std::string &strMark)
{
    m_setPreDefinedMarks.insert(strMark);
}

bool CCacheMgr::IsNeededCacheMarkExists(const std::string &strMark)
{
    return m_setPreDefinedMarks.count(strMark) > 0;
}

bool CCacheMgr::PreBuildCaches()
{
    class Local
    {
    public:
        Local(VTDNav *&vn)
            : m_vn(vn)
        {
            m_vn = CScdChecker::GetInstance()->GetExistingVtdNav();
        }

        ~Local()
        {
            m_vn = NULL;
        }

    private:
        VTDNav *&m_vn;

    } __local(m_vn);

    return PreBuildCachesInternal();
}

bool CCacheMgr::PreBuildCachesInternal()
{
    // �����
    pChecker = CScdChecker::GetInstance();
    // �����������
    assert(pChecker != NULL);

    CPreCheck preCheck;

    return (GetSSDSubstationCache() &&
            GetSCDCRC() &&
            GetHeaderCache() &&
            GetCommunicationCache() &&
            GetIEDCache() &&
            GetDataTypeTemplateCache() &&
            preCheck.CheckCommon());
}

// SCD CRC
bool CCacheMgr::GetSCDCRC()
{
    m_vn->toElement(ROOT);
    if (m_vn->toElement(FIRST_CHILD, L"Private"))
    {
        do
        {
            wstring type = VtdGetAttributeValue(m_vn, L"type");
            if (type.compare(L"Substation virtual terminal conection CRC") == 0)
            {
                m_SCDCache.crc = VtdGetTextValue(m_vn);
                break;
            }
        }while(m_vn->toElement(NEXT_SIBLING, L"Private"));
                    
        m_vn->toElement(PARENT);
    }
    return true;
}

// IED Cache
bool CCacheMgr::GetIEDCache()
{
    try
    {
        m_vn->toElement(ROOT);
        if (m_vn->toElement(FIRST_CHILD, L"IED"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Attr_R").bEnabled;
                ErrorType etAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Attr_R").etValue;
                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Attr_O").bEnabled;
                ErrorType etAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Attr_O").etValue;
                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Child").bEnabled;      
                ErrorType etChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Child").etValue;
                bool bParentChecked = CSCDIEDCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 1); // IEDԪ�غ���name��1������
                AddDueElementInfo(bParentChecked, bAttr_O, 4); // IEDԪ�غ���desc, type, manufacturer ��configVersion��4������
                AddDueElementInfo(bParentChecked, bChild, 1); // IEDԪ�غ���2����Ԫ��Servises��AccessPoint������AccessPointԪ��Ϊ��ѡ

                string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                if (!m_vn->hasAttr(L"name"))
                {
                    string msg = FormatXMLMessage("[IED name=%s]IEDԪ����ȱ��name����", name.c_str());
                    SCDIEDCheck(msg, bAttr_R, etAttr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s]IEDԪ����ȱ��desc����", name.c_str());
                    SCDIEDCheck(msg, bAttr_O, etAttr_O);
                }
                if (!m_vn->hasAttr(L"type"))
                {
                    string msg = FormatXMLMessage("[IED name=%s]IEDԪ����ȱ��type����", name.c_str());
                    SCDIEDCheck(msg, bAttr_O, etAttr_O);
                }
                if (!m_vn->hasAttr(L"manufacturer"))
                {
                    string msg = FormatXMLMessage("[IED name=%s]IEDԪ����ȱ��manufacturer����", name.c_str());
                    SCDIEDCheck(msg, bAttr_O, etAttr_O);
                }
                if (!m_vn->hasAttr(L"configVersion"))
                {
                    string msg = FormatXMLMessage("[IED name=%s]IEDԪ����ȱ��configVersion����", name.c_str());
                    SCDIEDCheck(msg, bAttr_O, etAttr_O);
                }

                setIntAddr.clear(); // ���IED�µ�intAddr set

                IEDCache iedCache;
                iedCache.name = VtdGetAttributeValue(m_vn, L"name");
                CreatInterval(iedCache.name);

                iedCache.offset = GetOffsetOfVTD();
                GetIEDCRC(iedCache);
                GetServisesCache(iedCache);
                GetLDCache(iedCache);

                // ��IED����Map��
                IEDMap::iterator it = m_SCDCache.mapIED.find(iedCache.name);
                if (it != m_SCDCache.mapIED.end())
                {
                    int row = pChecker->CalcRowNumberFromOffset(iedCache.offset);
                    string xml = pChecker->GetRowContentByOffset(iedCache.offset);
                    string msg = "IED���Ʋ�Ψһ:IED name=%s";
                    CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                        WtoA(iedCache.name).c_str());
                }
                else
                {
                    m_SCDCache.mapIED.insert(make_pair(iedCache.name, iedCache));
                }

            } while (m_vn->toElement(NEXT_SIBLING, L"IED"));
        }
    }
    catch(...)
    {
        return false;
    }
    return true;
}

// ȡ��Services��Ϣ
bool CCacheMgr::GetServisesCache(IEDCache &iedCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"Services"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_Child_O").bEnabled;
                ErrorType etChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_Child_O").etValue;
                bool bParentChecked = CSCDIEDServicesCheck::GetInstance()->IsEnabled();
                // ServisesԪ��Ӧ����SettingGroups, ConfDataSet, DynDataSet,ConfReportControl,ConfLogControl,ReportSettings, 
                // LogSettings, GSESettings,SMVSettings, GOOSE, GSSE, ��ConfLNs��12����Ԫ�ء�
                AddDueElementInfo(bParentChecked, bChild, 12);
                
                bool bHaveSettingGroups = false;
                bool bHaveConfDataSet = false;
                bool bHaveDynDataSet = false;
                bool bHaveConfReportControl = false;
                bool bHaveConfLogControl = false;
                bool bHaveReportSettings = false;
                bool bHaveLogSettings = false;
                bool bHaveGSESettings = false;
                bool bHaveSMVSettings = false;
                bool bHaveGOOSE = false;
                bool bHaveGSSE = false;
                bool bHaveConfLNs = false;

                if (m_vn->toElement(FIRST_CHILD))
                {
                    do 
                    {
                        wstring tag = VtdGetElementName(m_vn);
                        if (tag == L"SettingGroups")
                        {
                            bHaveSettingGroups = true;
                            bool bSCDIEDServicesCheck_SettingGroups_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_SettingGroups_Child_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_SettingGroups_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_SettingGroups_Child_O").etValue;
                            // SettingGroupsԪ�غ���SGEdit��ConfSG��2����Ԫ��
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_SettingGroups_Child, 2);

                            bool bHaveSGEdit = false;
                            bool bHaveConfSG = false;

                            if (m_vn->toElement(FIRST_CHILD))
                            {
                                do 
                                {
                                    wstring settingGroupsTag = VtdGetElementName(m_vn);
                                    if (settingGroupsTag == L"SGEdit")
                                    {
                                        bHaveSGEdit = true;
                                    }
                                    else if (settingGroupsTag == L"ConfSG")
                                    {
                                        bHaveConfSG = true;
                                    }

                                } while (m_vn->toElement(NEXT_SIBLING));
                                m_vn->toElement(PARENT);
                            }

                            if (!bHaveSGEdit)
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��SettingGroupsȱ��SGEdit��Ԫ��", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SettingGroups_Child, etSCDIEDServicesCheck_SettingGroups_Child);
                            }
                            if (!bHaveConfSG)
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��SettingGroupsȱ��ConfSG��Ԫ��", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SettingGroups_Child, etSCDIEDServicesCheck_SettingGroups_Child);
                            }
                                
                        }
                        else if (tag == L"ConfDataSet")
                        {
                            bHaveConfDataSet = true;
                            bool bSCDIEDServicesCheck_ConfDataSet_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfDataSet_Attr_R").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_ConfDataSet_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfDataSet_Attr_R").etValue;
                            bool bSCDIEDServicesCheck_ConfDataSet_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfDataSet_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_ConfDataSet_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfDataSet_Attr_O").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ConfDataSet_Attr_R, 1);//  ConfDataSetԪ�غ���max 1������
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ConfDataSet_Attr_O, 2);//  ConfDataSetԪ�غ���maxAttributes��modify��2������

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ConfDataSetԪ��ȱ��max����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfDataSet_Attr_R, etSCDIEDServicesCheck_ConfDataSet_Attr_R);
                            }
                            if (!m_vn->hasAttr(L"maxAttributes"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ConfDataSetԪ��ȱ��maxAttributes����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfDataSet_Attr_O, etSCDIEDServicesCheck_ConfDataSet_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"modify"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ConfDataSetԪ��ȱ��modify����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfDataSet_Attr_O, etSCDIEDServicesCheck_ConfDataSet_Attr_O);
                            }

                        }
                        else if (tag == L"DynDataSet")
                        {
                            bHaveDynDataSet = true;
                            bool bSCDIEDServicesCheck_DynDataSet_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_DynDataSet_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_DynDataSet_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_DynDataSet_Attr_O").etValue;
                            bool bSCDIEDServicesCheck_DynDataSet_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_DynDataSet_Attr_R").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_DynDataSet_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_DynDataSet_Attr_R").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_DynDataSet_Attr_R, 1);//  DynDataSetԪ�غ���max��1������
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_DynDataSet_Attr_O, 1);//  DynDataSetԪ�غ���maxAttributes��1������

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��DynDataSetԪ��ȱ��max����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_DynDataSet_Attr_R, etSCDIEDServicesCheck_DynDataSet_Attr_R);
                            }
                            if (!m_vn->hasAttr(L"maxAttributes"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��DynDataSetԪ��ȱ��maxAttributes����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_DynDataSet_Attr_O, etSCDIEDServicesCheck_DynDataSet_Attr_O);
                            }

                        }
                        else if (tag == L"ConfReportControl")
                        {
                            bHaveConfReportControl = true;
                            bool bSCDIEDServicesCheck_ConfReportControl_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfReportControl_Attr").bEnabled;
                            ErrorType etSCDIEDServicesCheck_ConfReportControl_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfReportControl_Attr").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ConfReportControl_Attr, 1);// ConfReportControlԪ�غ���max����

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ConfReportControlԪ��ȱ��max����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfReportControl_Attr, etSCDIEDServicesCheck_ConfReportControl_Attr);
                            }
                        }
                        else if (tag == L"ConfLogControl")
                        {
                            bHaveConfLogControl = true;
                            bool bSCDIEDServicesCheck_ConfLogControl_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfLogControl_Attr").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_ConfLogControl_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfLogControl_Attr").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ConfLogControl_Attr, 1);// ConfLogControlԪ�غ���max����

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ConfLogControlԪ��ȱ��max����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfLogControl_Attr, etSCDIEDServicesCheck_ConfLogControl_Attr);
                            }
                        }
                        else if (tag == L"ReportSettings")
                        {
                            bHaveReportSettings = true;
                            bool bSCDIEDServicesCheck_ReportSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ReportSettings_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_ReportSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ReportSettings_Attr_O").etValue;
                            // ReportSettingsԪ�غ���cbName, dataset, rptID, optFields, bufTime, trgOps��intgPd��7�����ԡ�
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ReportSettings_Attr_O, 7);

                            if (!m_vn->hasAttr(L"cbName"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ReportSettingsԪ��ȱ��cbName����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"datSet"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ReportSettingsԪ��ȱ��datSet����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"rptID"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ReportSettingsԪ��ȱ��rptID����", WtoA(iedCache.name).c_str());
                                    "ReportSettingsԪ��ȱ��rptID����";
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"optFields"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ReportSettingsԪ��ȱ��optFields����", WtoA(iedCache.name).c_str());
                                    "ReportSettingsԪ��ȱ��optFields����";
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"bufTime"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ReportSettingsԪ��ȱ��bufTime����", WtoA(iedCache.name).c_str());
                                    "ReportSettingsԪ��ȱ��bufTime����";
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"trgOps"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ReportSettingsԪ��ȱ��trgOps����", WtoA(iedCache.name).c_str());
                                    "ReportSettingsԪ��ȱ��trgOps����";
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"intgPd"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ReportSettingsԪ��ȱ��intgPd����", WtoA(iedCache.name).c_str());
                                    "ReportSettingsԪ��ȱ��intgPd����";
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                        }
                        else if (tag == L"LogSettings")
                        {
                            bHaveLogSettings = true;
                            bool bSCDIEDServicesCheck_LogSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_LogSettings_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_LogSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_LogSettings_Attr_O").etValue;
                            // LogSettingsԪ�غ���cbName, dataset, logEna, trgOps��intgPd��5������
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_LogSettings_Attr_O, 5);

                            if (!m_vn->hasAttr(L"cbName"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��LogSettingsԪ��ȱ��cbName����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_LogSettings_Attr_O, etSCDIEDServicesCheck_LogSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"datSet"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��LogSettingsԪ��ȱ��datSet����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_LogSettings_Attr_O, etSCDIEDServicesCheck_LogSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"logEna"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��LogSettingsԪ��ȱ��logEna����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_LogSettings_Attr_O, etSCDIEDServicesCheck_LogSettings_Attr_O);
                            }                            
                            if (!m_vn->hasAttr(L"trgOps"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��LogSettingsԪ��ȱ��trgOps����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_LogSettings_Attr_O, etSCDIEDServicesCheck_LogSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"intgPd"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��LogSettingsԪ��ȱ��intgPd����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_LogSettings_Attr_O, etSCDIEDServicesCheck_LogSettings_Attr_O);
                            }
                        }
                        else if (tag == L"GSESettings")
                        {
                            bHaveGSESettings = true;
                            bool bSCDIEDServicesCheck_GSESettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GSESettings_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_GSESettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GSESettings_Attr_O").etValue;
                            // GSESettingsԪ�غ���cbName, dataset, appID��dataLabel��4������
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_GSESettings_Attr_O, 4);

                            if (!m_vn->hasAttr(L"cbName"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��GSESettingsԪ��ȱ��cbName����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GSESettings_Attr_O, etSCDIEDServicesCheck_GSESettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"datSet"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��GSESettingsԪ��ȱ��datSet����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GSESettings_Attr_O, etSCDIEDServicesCheck_GSESettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"appID"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��GSESettingsԪ��ȱ��appID����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GSESettings_Attr_O, etSCDIEDServicesCheck_GSESettings_Attr_O);
                            }                            
                            if (!m_vn->hasAttr(L"dataLabel"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��GSESettingsԪ��ȱ��dataLabel����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GSESettings_Attr_O, etSCDIEDServicesCheck_GSESettings_Attr_O);
                            }
                        }
                        else if (tag == L"SMVSettings")
                        {
                            bHaveSMVSettings = true;
                            bool bSCDIEDServicesCheck_SMVSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_SMVSettings_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_SMVSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_SMVSettings_Attr_O").etValue;
                            // SMVSettingsԪ�غ���cbName, dataset, svILD, optFields�� smpRate��5������
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_SMVSettings_Attr_O, 5);

                            if (!m_vn->hasAttr(L"cbName"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��SMVSettingsԪ��ȱ��cbName����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SMVSettings_Attr_O, etSCDIEDServicesCheck_SMVSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"datSet"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��SMVSettingsԪ��ȱ��datSet����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SMVSettings_Attr_O, etSCDIEDServicesCheck_SMVSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"svID"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��SMVSettingsԪ��ȱ��svID����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SMVSettings_Attr_O, etSCDIEDServicesCheck_SMVSettings_Attr_O);
                            }                            
                            if (!m_vn->hasAttr(L"optFields"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��SMVSettingsԪ��ȱ��optFields����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SMVSettings_Attr_O, etSCDIEDServicesCheck_SMVSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"smpRate"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��SMVSettingsԪ��ȱ��smpRate����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SMVSettings_Attr_O, etSCDIEDServicesCheck_SMVSettings_Attr_O);
                            }
                        }
                        else if (tag == L"GOOSE")
                        {
                            bHaveGOOSE = true;
                            bool bSCDIEDServicesCheck_GOOSE_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GOOSE_Attr").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_GOOSE_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GOOSE_Attr").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_GOOSE_Attr, 1);// GOOSEԪ�غ���max����

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��GOOSEԪ��ȱ��max����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GOOSE_Attr, etSCDIEDServicesCheck_GOOSE_Attr);
                            }
                        }
                        else if (tag == L"GSSE")
                        {
                            bHaveGSSE = true;
                            bool bSCDIEDServicesCheck_GSSE_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GSSE_Attr").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_GSSE_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GSSE_Attr").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_GSSE_Attr, 1);// GSSEԪ�غ���max����

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��GSSEԪ��ȱ��max����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GSSE_Attr, etSCDIEDServicesCheck_GSSE_Attr);
                            }
                        }
                        else if (tag == L"ConfLNs")
                        {
                            bHaveConfLNs = true;
                            bool bSCDIEDServicesCheck_ConfLNs_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfLNs_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_ConfLNs_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfLNs_Attr_O").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ConfLNs_Attr_O, 2);// ConfLNsԪ�غ���fixPrefix��fixLnInst����

                            if (!m_vn->hasAttr(L"fixPrefix"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ConfLNsԪ��ȱ��fixPrefix����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfLNs_Attr_O, etSCDIEDServicesCheck_ConfLNs_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"fixLnInst"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED��Services��Ԫ��ConfLNsԪ��ȱ��fixLnInst����", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfLNs_Attr_O, etSCDIEDServicesCheck_ConfLNs_Attr_O);
                            }
                        }

                    } while (m_vn->toElement(NEXT_SIBLING));

                    m_vn->toElement(PARENT);
                }

                if (!bHaveSettingGroups)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��SettingGroups��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveConfDataSet)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��ConfDataSet��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveDynDataSet)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��DynDataSet��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveConfReportControl)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��ConfReportControl��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveConfLogControl)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��ConfLogControl��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveReportSettings)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��ReportSettings��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveLogSettings)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��LogSettings��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveGSESettings)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��GSESettings��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveSMVSettings)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��SMVSettings��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveGOOSE)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��GOOSE��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveGSSE)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��GSSE��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveConfLNs)
                {
                    string msg = FormatXMLMessage("[IED name=%s]ServicesԪ��ȱ��ConfLNs��Ԫ��", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }

            } while (m_vn->toElement(NEXT_SIBLING, L"Services"));
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::CreatInterval(wstring &iedName)
{
    if (IsNeededCacheMarkExists("AllBay"))
    {
        if (iedName.find(L"PM") != wstring::npos)
        {
            m_PMDevice.insert(iedName);
        }

        //static CRegexpT <wchar_t> regexp(L"[A-Z](?<Bay>[A-Z][0-9]+)");
        //MatchResult result = regexp.Match(iedName.c_str());
        //while(result.IsMatched())
        //{
        //    int gN = regexp.GetNamedGroupNumber(L"Bay");
        //    if (gN >= 0)
        //    {
        //        int len = result.GetGroupEnd(gN) - result.GetGroupStart(gN);
        //        wstring bay = iedName.substr(result.GetGroupStart(gN), len);
        //        m_SCDCache.mapAllBay[bay].insert(iedName);
        //    }
        //    result = regexp.Match(iedName.c_str(), result.GetEnd());
        //}
    }
    else if (IsNeededCacheMarkExists("SwitchBay"))
    {
        int size = iedName.size();
        if (size > 2 && (iedName[1] == L'L'))
        {
            if ((iedName[size-1] == L'A') ||
                (iedName[size-1] == L'B'))
            {
                m_SCDCache.mapSwitchBay[iedName.substr(2, size-3)].insert(iedName);
            }
            else
            {
                m_SCDCache.mapSwitchBay[iedName.substr(2, size-1)].insert(iedName);
            }
        }
    }


    return true;
}

bool CCacheMgr::GetIEDCRC(IEDCache &iedCache)
{
    if (m_vn->toElement(FIRST_CHILD, L"Private"))
    {
        do 
        {
            wstring type = VtdGetAttributeValue(m_vn, L"type");
            if (type.compare(L"IED virtual terminal conection CRC") == 0)
            {
                iedCache.CRC = VtdGetTextValue(m_vn);
            }
        } while (m_vn->toElement(NEXT_SIBLING, L"Private"));
        m_vn->toElement(PARENT);
    }
    return true;
}

bool CCacheMgr::GetLDCache(IEDCache &iedCache)
{
    try
    {
        bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Child").bEnabled; 
        ErrorType etSCDIEDCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Child").etValue;
        if (m_vn->toElement(FIRST_CHILD, L"AccessPoint"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Attr_R").bEnabled;
                ErrorType etSCDIEDAccessPointCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Attr_R").etValue;
                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Attr_O").bEnabled;
                ErrorType etSCDIEDAccessPointCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Attr_O").etValue;
                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Child").bEnabled;
                ErrorType etSCDIEDAccessPointCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Child").etValue;
                bool bParentChecked = CSCDIEDAccessPointCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 1); // AccessPointԪ�غ���name��4������
                AddDueElementInfo(bParentChecked, bAttr_O, 3); // AccessPointԪ�غ���desc, router��clock��4������
                AddDueElementInfo(bParentChecked, bChild, 1); // AccessPointԪ�غ���Server��Ԫ��

                string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                if (!m_vn->hasAttr(L"name"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]AccessPointԪ��ȱ��name����", WtoA(iedCache.name).c_str(), name.c_str());
                    SCDIEDAccessPointCheck(msg, bAttr_R, etSCDIEDAccessPointCheck_Attr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]AccessPointԪ��ȱ��desc����", WtoA(iedCache.name).c_str(), name.c_str());
                    SCDIEDAccessPointCheck(msg, bAttr_O, etSCDIEDAccessPointCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"router"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]AccessPointԪ��ȱ��router����", WtoA(iedCache.name).c_str(), name.c_str());
                    SCDIEDAccessPointCheck(msg, bAttr_O, etSCDIEDAccessPointCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"clock"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]AccessPointԪ��ȱ��clock����", WtoA(iedCache.name).c_str(), name.c_str());
                    SCDIEDAccessPointCheck(msg, bAttr_O, etSCDIEDAccessPointCheck_Attr_O);
                }

                wstring apName = VtdGetAttributeValue(m_vn, L"name");

                int offset = GetOffsetOfVTD();

                if (m_vn->toElement(FIRST_CHILD, L"Server"))
                {
                    bool bSCDIEDAccessPointCheck_Server_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Server_Attr_O").bEnabled;
                    ErrorType etSCDIEDAccessPointCheck_Server_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Server_Attr_O").etValue;
                    bool bSCDIEDAccessPointCheck_Server_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Server_Child").bEnabled;
                    ErrorType etSCDIEDAccessPointCheck_Server_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Server_Child").etValue;
                    AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_Server_Attr, 2); // ServerԪ�غ���timeout��desc��2������
                    AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_Server_Child, 2); // ServerԪ�غ�����Ԫ��Authentication��֤Ԫ�ء�LDevice�߼��豸Ԫ�ء�Association����Ԫ�ء�����Authentication��LDeviceΪ����Ԫ��

                    if (!m_vn->hasAttr(L"timeout"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]ServerԪ��ȱ��timeout����", WtoA(iedCache.name).c_str(), name.c_str());
                        SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Server_Attr, etSCDIEDAccessPointCheck_Server_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]ServerԪ��ȱ��desc����", WtoA(iedCache.name).c_str(), name.c_str());
                        SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Server_Attr, etSCDIEDAccessPointCheck_Server_Attr_O);
                    }

                    if (m_vn->toElement(FIRST_CHILD, L"Authentication"))
                    {
                        bool bSCDIEDAccessPointCheck_Authentication_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Authentication_Attr_O").bEnabled;
                        ErrorType etSCDIEDAccessPointCheck_Authentication_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Authentication_Attr_O").etValue;
                        // AuthenticationԪ�غ���none, password, weak, strong �� certificate��5������
                        AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_Authentication_Attr_O, 5); 
                        if (!m_vn->hasAttr(L"none"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Authenticationȱ��none����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Authentication_Attr_O, etSCDIEDAccessPointCheck_Authentication_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"password"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Authenticationȱ��password����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Authentication_Attr_O, etSCDIEDAccessPointCheck_Authentication_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"weak"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Authenticationȱ��weak����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Authentication_Attr_O, etSCDIEDAccessPointCheck_Authentication_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"strong"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Authenticationȱ��strong����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Authentication_Attr_O, etSCDIEDAccessPointCheck_Authentication_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"certificate"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Authenticationȱ��certificate����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Authentication_Attr_O, etSCDIEDAccessPointCheck_Authentication_Attr_O);
                        }
                        
                        m_vn->toElement(PARENT); // return to parent
                    }
                    else
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]ServerԪ��ȱ��Authentication��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str());
                        SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Server_Child, etSCDIEDAccessPointCheck_Server_Child);
                    }

                    if (m_vn->toElement(FIRST_CHILD, L"LDevice"))
                    {
                        do 
                        {
                            bool bSCDIEDAccessPointCheck_LDevice_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_LDevice_Attr_R").bEnabled;
                            ErrorType etSCDIEDAccessPointCheck_LDevice_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_LDevice_Attr_R").etValue;
                            bool bSCDIEDAccessPointCheck_LDevice_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_LDevice_Attr_O").bEnabled;
                            ErrorType etSCDIEDAccessPointCheck_LDevice_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_LDevice_Attr_O").etValue;
                            bool bSCDIEDAccessPointCheck_LDevice_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_LDevice_Child").bEnabled;
                            ErrorType etSCDIEDAccessPointCheck_LDevice_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_LDevice_Child").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_LDevice_Attr_R, 1); // LDeviceԪ�غ���inst��1������
                            AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_LDevice_Attr_O, 1); // LDeviceԪ�غ���desc��1������
                            AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_LDevice_Child, 1); // LDeviceԪ�غ�����Ԫ��LN0���߼��ڵ�ʵ��Ԫ�ء�LN�����߼��ڵ�ʵ��Ԫ�ء�AccessControl���ʿ���ʵ��Ԫ�ء�LN0Ϊ��ѡԪ��
                            
                            string inst = WtoA(VtdGetAttributeValue(m_vn, L"inst")).c_str();
                            if (!m_vn->hasAttr(L"inst"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LDeviceȱ��inst����", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_LDevice_Attr_R, etSCDIEDAccessPointCheck_LDevice_Attr_R);
                            }
                            if (!m_vn->hasAttr(L"desc"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LDeviceȱ��desc����", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_LDevice_Attr_O, etSCDIEDAccessPointCheck_LDevice_Attr_O);
                            }
                            if (m_vn->toElement(FIRST_CHILD, L"LN0"))
                            {
                                bool bSCDIEDLNCheck_LN0_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN0_Attr_R").bEnabled;
                                ErrorType etSCDIEDLNCheck_LN0_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN0_Attr_R").etValue;
                                bool bSCDIEDLNCheck_LN0_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN0_Attr_O").bEnabled;
                                ErrorType etSCDIEDLNCheck_LN0_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN0_Attr_O").etValue;
                                bool bSCDIEDLNCheck_LN0_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN0_Child_O").bEnabled;
                                ErrorType etSCDIEDLNCheck_LN0_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN0_Child_O").etValue;
                                bool bLNParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                                AddDueElementInfo(bLNParentChecked, bSCDIEDLNCheck_LN0_Attr_R, 3); // LN0Ԫ�غ���lnClass, InType, inst��desc��4������
                                AddDueElementInfo(bLNParentChecked, bSCDIEDLNCheck_LN0_Attr_O, 1); // LN0Ԫ�غ���desc��1������
                                // LN0Ԫ�غ�����Ԫ��DataSet���ݼ�ʵ��Ԫ�ء�ReportControl�������ʵ��Ԫ�ء�LogControl��־����ʵ��Ԫ�ء�DOI���ݶ���ʵ��Ԫ�ء�
                                // Inputs��Ϣ����ʵ��Ԫ�ء�GSEControlGSE����ʵ��Ԫ�ء�SampledValueControl����ֵ����ʵ��Ԫ�ء�SettingControl��ֵ����ʵ��Ԫ�ء�
                                // SCLControlSCL����Ԫ�ء�Log��־Ԫ��
                                AddDueElementInfo(bLNParentChecked, bSCDIEDLNCheck_LN0_Child, 10); 

                                if (!m_vn->hasAttr(L"desc"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0Ԫ��ȱ��desc����", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Attr_O, etSCDIEDLNCheck_LN0_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"inst"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0Ԫ��ȱ��inst����", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Attr_R, etSCDIEDLNCheck_LN0_Attr_R);
                                }
                                if (!m_vn->hasAttr(L"lnType"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0Ԫ��ȱ��lnType����", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Attr_R, etSCDIEDLNCheck_LN0_Attr_R);
                                }
                                if (!m_vn->hasAttr(L"lnClass"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0Ԫ��ȱ��lnClass����", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Attr_R, etSCDIEDLNCheck_LN0_Attr_R);
                                }
                                
                                // child
                                if (m_vn->toElement(FIRST_CHILD, L"DataSet"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0ȱ��DataSet��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"ReportControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0ȱ��ReportControl��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"LogControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0ȱ��LogControl��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"DOI"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0ȱ��DOI��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"Inputs"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0ȱ��Inputs��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"GSEControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0ȱ��GSEControl��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"SampledValueControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0ȱ��SampledValueControl��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"SettingControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0ȱ��SettingControl��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"SCLControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0ȱ��SCLControl��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"Log"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0ȱ��Log��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                
                                m_vn->toElement(PARENT);
                            }
                            else
                            {
                                string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]Server����Ԫ��LDeviceȱ��LN0��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_LDevice_Child, etSCDIEDAccessPointCheck_LDevice_Child);
                            }

                            LDCache ldCache;
                            ldCache.iedName = iedCache.name;
                            ldCache.apName = apName;
                            ldCache.inst = VtdGetAttributeValue(m_vn, L"inst");
                            ldCache.key = iedCache.name + L"." + apName + L"." + ldCache.inst;
                            int ldOffset = GetOffsetOfVTD();

                            GetDataSetCache(ldCache);
                            GetGSEControlCache(ldCache);
                            GetReportControlCache(ldCache);
                            GetLogControlCache(ldCache);
                            GetSCLControlCache(ldCache);
                            GetSettingControlCache(ldCache);
                            GetSampledValueControlCache(ldCache);
                            GetInputsCache(ldCache);
                            GetLogicNodeInstanceCache(ldCache);

                            // insert ld into apCache
                            LDMap::iterator it = iedCache.mapLD.find(ldCache.key);
                            if (it != iedCache.mapLD.end())
                            {
                                int row = pChecker->CalcRowNumberFromOffset(ldOffset);
                                string xml = pChecker->GetRowContentByOffset(ldOffset);
                                string msg = "�߼��豸��Ψһ:IED name=%s, LDevice inst=%s";
                                CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                                    WtoA(iedCache.name).c_str(), WtoA(ldCache.inst).c_str());
                            } 
                            else
                            {
                                iedCache.mapLD.insert(make_pair(ldCache.key, ldCache));
                            }

                        } while (m_vn->toElement(NEXT_SIBLING, L"LDevice"));
                        m_vn->toElement(PARENT);
                    }
                    else
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]ServerԪ��ȱ��LDevice��Ԫ��", WtoA(iedCache.name).c_str(), name.c_str());
                        SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Server_Child, etSCDIEDAccessPointCheck_Server_Child);
                    }

                    if (m_vn->toElement(FIRST_CHILD, L"Association"))
                    {
                        bool bSCDIEDAccessPointCheck_Association_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Association_Attr_R").bEnabled;
                        ErrorType etSCDIEDAccessPointCheck_Association_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Association_Attr_R").etValue;
                        bool bSCDIEDAccessPointCheck_Association_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Association_Attr_O").bEnabled;
                        ErrorType etSCDIEDAccessPointCheck_Association_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Association_Attr_O").etValue;
                        // AssociationԪ�غ���kind, associationID, iedName, IdInst, InClass, prefix��InInst��7������
                        AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_Association_Attr_R, 5);  //  desc="AssociationԪ�غ���kind,  iedName, ldInst, lnClass, ��lnInst��5������"
                        AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_Association_Attr_O, 2); //  desc="AssociationԪ�غ���associationID,  prefix��2������"
                        if (!m_vn->hasAttr(L"kind"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Associationȱ��kind����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_R, etSCDIEDAccessPointCheck_Association_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"associationID"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Associationȱ��associationID����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_O, etSCDIEDAccessPointCheck_Association_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"iedName"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Associationȱ��iedName����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_R, etSCDIEDAccessPointCheck_Association_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"ldInst"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Associationȱ��ldInst����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_R, etSCDIEDAccessPointCheck_Association_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"lnClass"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Associationȱ��lnClass����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_R, etSCDIEDAccessPointCheck_Association_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"prefix"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Associationȱ��prefix����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_O, etSCDIEDAccessPointCheck_Association_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"lnInst"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server����Ԫ��Associationȱ��lnInst����", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_R, etSCDIEDAccessPointCheck_Association_Attr_R);
                        }

                        m_vn->toElement(PARENT);
                    }

                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]AccessPointԪ��ȱ����Ԫ��Server", WtoA(iedCache.name).c_str(), name.c_str());
                    SCDIEDAccessPointCheck(msg, bChild, etSCDIEDAccessPointCheck_Child);
                }

                if (iedCache.apSet.count(apName) != 1)
                {
                    iedCache.apSet.insert(apName);
                }
                else
                {
                    int row = pChecker->CalcRowNumberFromOffset(offset);
                    string xml = pChecker->GetRowContentByOffset(offset);
                    string msg = "AccessPoint��Ψһ:IED name=%s, AccessPoint name=%s";
                    CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                        WtoA(iedCache.name).c_str(), WtoA(apName).c_str());
                }

            } while (m_vn->toElement(NEXT_SIBLING, L"AccessPoint"));
            m_vn->toElement(PARENT);
        }
        else
        {
            string msg = "IEDԪ��ȱ��AccessPoint��Ԫ��";
            SCDIEDCheck(msg, bChild, etSCDIEDCheck_Child);
        }
    }
    catch(...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetDataSetCache(LDCache &ldCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"LN0"))
        {
            if (m_vn->toElement(FIRST_CHILD, L"DataSet"))
            {
                do 
                {
                    // �ж������Ƿ�ѡ��
                    bool bSCDIEDLNCheck_DataSet_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DataSet_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_DataSet_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DataSet_Attr_R").etValue;
                    bool bSCDIEDLNCheck_DataSet_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DataSet_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_DataSet_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DataSet_Attr_O").etValue;
                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DataSet_Attr_R, 1); // DataSetԪ�غ���name��1������
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DataSet_Attr_O, 1); // DataSetԪ�غ���desc��1������

                    string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DataSet name=%s]DataSetԪ��ȱ��name����", WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_DataSet_Attr_R, etSCDIEDLNCheck_DataSet_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DataSet name=%s]DataSetԪ��ȱ��desc����", WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_DataSet_Attr_O, etSCDIEDLNCheck_DataSet_Attr_O);
                    }

                    DataSetCache dsCache;
                    dsCache.iedName = ldCache.iedName;
                    dsCache.ldInst = ldCache.inst;
                    dsCache.name = VtdGetAttributeValue(m_vn, L"name");
                    int offset = GetOffsetOfVTD();

                    GetFCDACache(dsCache);
                    DataSetMap::iterator it = ldCache.mapDataSet.find(dsCache.name);
                    if (it != ldCache.mapDataSet.end())
                    {
                        int row = pChecker->CalcRowNumberFromOffset(offset);
                        string xml = pChecker->GetRowContentByOffset(offset);
                        string msg = "���ݼ������ظ�:IED name=%s LDevice inst=%s DataSet name=%s";
                        CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(dsCache.name).c_str());
                    } 
                    else
                    {
                        ldCache.mapDataSet.insert(make_pair(dsCache.name, dsCache));
                    }
                } while (m_vn->toElement(NEXT_SIBLING, L"DataSet"));
                m_vn->toElement(PARENT);
            }
            m_vn->toElement(PARENT);
        }
    }
    catch(...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetFCDACache(DataSetCache &dsCache)
{
    if (!IsNeededCacheMarkExists("FCDA"))
    {
        return true;
    }
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"FCDA"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bSCDIEDLNCheck_FCDA_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_FCDA_Attr_R").bEnabled;
                ErrorType etSCDIEDLNCheck_FCDA_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_FCDA_Attr_R").etValue;
                bool bSCDIEDLNCheck_FCDA_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_FCDA_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_FCDA_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_FCDA_Attr_O").etValue;
                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_FCDA_Attr_R, 1); // FCDAԪ�غ���fc��1������
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_FCDA_Attr_O, 6); // FCDAԪ�غ���IdInst, prefix, InClass, InInst, doName, daName��6������
                             
                FCDACache fcdaCache;
                fcdaCache.ldInst = VtdGetAttributeValue(m_vn, L"ldInst");
                fcdaCache.lnInst = VtdGetAttributeValue(m_vn, L"lnInst");
                fcdaCache.lnClass = VtdGetAttributeValue(m_vn, L"lnClass");
                fcdaCache.prefix = VtdGetAttributeValue(m_vn, L"prefix");
                fcdaCache.doName = VtdGetAttributeValue(m_vn, L"doName");
                fcdaCache.daName = VtdGetAttributeValue(m_vn, L"daName");
                fcdaCache.fc = VtdGetAttributeValue(m_vn, L"fc");
                fcdaCache.key = fcdaCache.ldInst + L"/" + fcdaCache.prefix + fcdaCache.lnClass 
                    + fcdaCache.lnInst + L"." + fcdaCache.doName;
                if (fcdaCache.daName.size() > 0)
                {
                    fcdaCache.key = fcdaCache.key + L"." + fcdaCache.daName;
                }
                fcdaCache.offset = GetOffsetOfVTD();
                 // �����Լ��
                if (!m_vn->hasAttr(L"ldInst"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDAȱ��ldInst����", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"prefix"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDAȱ��prefix����", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"lnClass"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDAȱ��lnClass����", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"lnInst"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDAȱ��lnInst����", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"doName"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDAȱ��doName����", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"daName"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDAȱ��daName����", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"fc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDAȱ��fc����", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_R, etSCDIEDLNCheck_FCDA_Attr_R);
                }

                FCDAMap::iterator it = dsCache.mapFCDA.find(fcdaCache.key);
                if (it != dsCache.mapFCDA.end())
                {
                    int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
                    string xml = pChecker->GetRowContentByRowNumber(row);
                    string msg = "FCDA ��Ψһ:IED name=%s LDevice inst=%s DataSet name=%s FCDA=%s";
                    CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), WtoA(dsCache.name).c_str(), WtoA(fcdaCache.key).c_str());
                } 
                else
                {
                    dsCache.mapFCDA.insert(make_pair(fcdaCache.key, fcdaCache));
                }
            } while (m_vn->toElement(NEXT_SIBLING, L"FCDA"));
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetGSEControlCache(LDCache &ldCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"LN0"))
        {
            if (m_vn->toElement(FIRST_CHILD, L"GSEControl"))
            {
                do 
                {
                    // �ж������Ƿ�ѡ��
                    bool bSCDIEDLNCheck_GSEControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_GSEControl_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_GSEControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_GSEControl_Attr_R").etValue;
                    bool bSCDIEDLNCheck_GSEControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_GSEControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_GSEControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_GSEControl_Attr_O").etValue;
                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // GSEControlԪ�غ���name, desc, datSet, confRev, type��appID��6������
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_GSEControl_Attr_R, 2); 
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_GSEControl_Attr_O, 4); 

                    string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControlȱ��name����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_GSEControl_Attr_R, etSCDIEDLNCheck_GSEControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControlȱ��desc����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_GSEControl_Attr_O, etSCDIEDLNCheck_GSEControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"datSet"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControlȱ��datSet����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_GSEControl_Attr_O, etSCDIEDLNCheck_GSEControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"confRev"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControlȱ��confRev����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_GSEControl_Attr_O, etSCDIEDLNCheck_GSEControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"type"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControlȱ��type����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_GSEControl_Attr_O, etSCDIEDLNCheck_GSEControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"appID"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControlȱ��appID����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_GSEControl_Attr_R, etSCDIEDLNCheck_GSEControl_Attr_R);
                    }

                    ControlBlockInfoCache gseCB;
                    gseCB.name = VtdGetAttributeValue(m_vn, L"name");
                    gseCB.appID = VtdGetAttributeValue(m_vn, L"appID");
                    gseCB.dataSetName = VtdGetAttributeValue(m_vn, L"datSet");
                    gseCB.offset = GetOffsetOfVTD();

                    ControlIdUniqueCheck(gseCB.appID);

                    ControlBlockInfoMap::iterator it = ldCache.mapGSEControl.find(gseCB.name);
                    if (it != ldCache.mapGSEControl.end())
                    {
                        int row = pChecker->CalcRowNumberFromOffset(gseCB.offset);
                        string xml = pChecker->GetRowContentByOffset(gseCB.offset);
                        string msg = "Goose���ƿ�����ظ�:IED name=%s LDevice inst=%s ControlBlock name=%s";
                        CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(gseCB.name).c_str());
                    } 
                    else
                    {
                        ldCache.mapGSEControl.insert(make_pair(gseCB.name, gseCB));
                    }
                } while (m_vn->toElement(NEXT_SIBLING, L"GSEControl"));
                m_vn->toElement(PARENT);
            }
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CCacheMgr::GetReportControlCache(LDCache &ldCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"LN0"))
        {
            if (m_vn->toElement(FIRST_CHILD, L"ReportControl"))
            {
                do 
                {
                    // �ж������Ƿ�ѡ��
                    bool bSCDIEDLNCheck_ReportControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_ReportControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Attr_R").etValue;
                    bool bSCDIEDLNCheck_ReportControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_ReportControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Attr_O").etValue;
                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // ReportControlԪ�غ���name, desc, datSet, intgPd, rptID, confRev, buffered��bufTime��8������
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_ReportControl_Attr_R, 2); //  desc="ReportControlԪ�غ���name, confRev��2������"
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_ReportControl_Attr_O, 6);  //  desc="ReportControlԪ�غ���desc, datSet, intgPd, rptID, buffered��bufTime��6������"

                    string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControlȱ��name����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_R, etSCDIEDLNCheck_ReportControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControlȱ��desc����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"datSet"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControlȱ��datSet����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"intgPd"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControlȱ��intgPd����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"rptID"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControlȱ��rptID����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"confRev"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControlȱ��confRev����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_R, etSCDIEDLNCheck_ReportControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"buffered"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControlȱ��buffered����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"bufTime"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControlȱ��bufTime����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    
                    // Child
                    bool bSCDIEDLNCheck_ReportControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Child").bEnabled;
                    ErrorType etSCDIEDLNCheck_ReportControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Child").etValue;
                    // ReportControlԪ�غ�����Ԫ�أ�TrgOps����ѡ��Ԫ�ء�OptFieldsѡ��Ԫ�ء�RptEnabled����ʹ��Ԫ�ء�����OptFieldsԪ��Ϊ��ѡ
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_ReportControl_Child, 1); 

                    bool bHave_ReportControl_OptFields = false;
                    if (m_vn->toElement(FIRST_CHILD))
                    {
                        do 
                        {
                            wstring tag = VtdGetElementName(m_vn);
                            if (tag == L"OptFields")
                            {
                                bHave_ReportControl_OptFields = true;
                                bool bSCDIEDLNCheck_OptFields_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_OptFields_Attr_O").bEnabled;
                                ErrorType etSCDIEDLNCheck_OptFields_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_OptFields_Attr_O").etValue;
                                // OptFieldsԪ�غ���seqNum, timeStamp, dataSet, reasonCode, dataRef, bufOvfl, entryID, configRef��segmentation��9������
                                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_OptFields_Attr, 9);
                                if (!m_vn->hasAttr(L"seqNum"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��OptFieldsԪ��ȱ��seqNum����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"timeStamp"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��OptFieldsԪ��ȱ��timeStamp����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"dataSet"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��OptFieldsԪ��ȱ��dataSet����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"reasonCode"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��OptFieldsԪ��ȱ��reasonCode����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"dataRef"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��OptFieldsԪ��ȱ��dataRef����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"bufOvfl"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��OptFieldsԪ��ȱ��bufOvfl����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"entryID"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��OptFieldsԪ��ȱ��entryID����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"configRef"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��OptFieldsԪ��ȱ��configRef����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"segmentation"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��OptFieldsԪ��ȱ��segmentation����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }

                            }
                            else if (tag == L"TrgOps")
                            {
                                bool bSCDIEDLNCheck_TrgOps_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_TrgOps_Attr_O").bEnabled;
                                ErrorType etSCDIEDLNCheck_TrgOps_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_TrgOps_Attr_O").etValue;
                                 // TrgOpsԪ�غ���dchg, qchg, dupd��period��4������
                                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_TrgOps_Attr, 4);
                                // CheckTrgOpsAttr(bSCDIEDLNCheck_TrgOps_Attr);
                                if (!m_vn->hasAttr(L"dchg"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��TrgOpsԪ��ȱ��dchg����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"qchg"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��TrgOpsԪ��ȱ��qchg����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"dupd"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��TrgOpsԪ��ȱ��dupd����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"period"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��TrgOpsԪ��ȱ��period����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr_O);
                                }
                            }
                            else if (tag == L"RptEnabled")
                            {
                                bool bSCDIEDLNCheck_RptEnabled_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_RptEnabled_Attr_O").bEnabled;
                                ErrorType etSCDIEDLNCheck_RptEnabled_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_RptEnabled_Attr_O").etValue;
                                bool bSCDIEDLNCheck_RptEnabled_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_RptEnabled_Child_O").bEnabled;
                                ErrorType etSCDIEDLNCheck_RptEnabled_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_RptEnabled_Child_O").etValue;
                                // RptEnabledԪ�غ���desc��max��2������
                                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_RptEnabled_Attr_O, 2);
                                // RptEnabledԪ�غ�����Ԫ�أ�ClientLN������ƿ�ͻ���Ԫ��
                                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_RptEnabled_Child, 1);
                                if (!m_vn->hasAttr(L"desc"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��RptEnabledԪ��ȱ��desc����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_RptEnabled_Attr_O, etSCDIEDLNCheck_RptEnabled_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"max"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��RptEnabledԪ��ȱ��max����", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_RptEnabled_Attr_O, etSCDIEDLNCheck_RptEnabled_Attr_O);
                                }

                                if (m_vn->toElement(FIRST_CHILD, L"ClientLN"))
                                {
                                    do 
                                    {
                                        bool bSCDIEDLNCheck_ClientLN_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ClientLN_Attr_R").bEnabled;
                                        ErrorType etSCDIEDLNCheck_ClientLN_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ClientLN_Attr_R").etValue;
                                        bool bSCDIEDLNCheck_ClientLN_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ClientLN_Attr_O").bEnabled;
                                        ErrorType etSCDIEDLNCheck_ClientLN_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ClientLN_Attr_O").etValue;
                                        // ClientLNԪ�غ���ideName, IdInst, prefix, InClass��InInst��5������
                                        AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_ClientLN_Attr_R, 4);
                                        AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_ClientLN_Attr_O, 1); //prefix
                                        if (!m_vn->hasAttr(L"iedName"))
                                        {
                                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl��RptEnabled����Ԫ��ClientLNȱ��iedName����", 
                                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ClientLN_Attr_R, etSCDIEDLNCheck_ClientLN_Attr_R);
                                        }
                                        if (!m_vn->hasAttr(L"ldInst"))
                                        {
                                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl��RptEnabled����Ԫ��ClientLNȱ��ldInst����", 
                                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ClientLN_Attr_R,etSCDIEDLNCheck_ClientLN_Attr_R);
                                        }
                                        if (!m_vn->hasAttr(L"prefix"))
                                        {
                                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl��RptEnabled����Ԫ��ClientLNȱ��prefix����", 
                                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ClientLN_Attr_O, etSCDIEDLNCheck_ClientLN_Attr_O);
                                        }
                                        if (!m_vn->hasAttr(L"lnClass"))
                                        {
                                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl��RptEnabled����Ԫ��ClientLNȱ��lnClass����", 
                                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ClientLN_Attr_R, etSCDIEDLNCheck_ClientLN_Attr_R);
                                        }
                                        if (!m_vn->hasAttr(L"lnInst"))
                                        {
                                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl��RptEnabled����Ԫ��ClientLNȱ��lnInst����", 
                                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ClientLN_Attr_R, etSCDIEDLNCheck_ClientLN_Attr_R);
                                        }

                                    } while (m_vn->toElement(NEXT_SIBLING, L"ClientLN"));
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl����Ԫ��RptEnabledȱ��ClientLN��Ԫ��", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_RptEnabled_Child, etSCDIEDLNCheck_RptEnabled_Child_O);
                                }

                            }

                        } while (m_vn->toElement(NEXT_SIBLING));
                        m_vn->toElement(PARENT);
                    }

                    if (!bHave_ReportControl_OptFields)
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControlԪ��ȱ��OptFields��Ԫ��", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Child, etSCDIEDLNCheck_ReportControl_Child);
                    }

                    ControlBlockInfoCache CB;
                    CB.name = VtdGetAttributeValue(m_vn, L"name");
                    CB.appID = VtdGetAttributeValue(m_vn, L"rptID");
                    CB.dataSetName = VtdGetAttributeValue(m_vn, L"datSet");
                    CB.offset = GetOffsetOfVTD();

                    ControlBlockInfoMap::iterator it = ldCache.mapReportControl.find(CB.name);
                    if (it != ldCache.mapReportControl.end())
                    {
                        int row = pChecker->CalcRowNumberFromOffset(CB.offset);
                        string xml = pChecker->GetRowContentByOffset(CB.offset);
                        string msg = "Report���ƿ�����ظ�:IED name=%s LDevice inst=%s ControlBlock name=%s";
                        CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(CB.name).c_str());
                    } 
                    else
                    {
                        ldCache.mapReportControl.insert(make_pair(CB.name, CB));
                    }
                } while (m_vn->toElement(NEXT_SIBLING, L"ReportControl"));
                m_vn->toElement(PARENT);
            }
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetLogControlCache(LDCache &ldCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"LN0"))
        {
            if (m_vn->toElement(FIRST_CHILD, L"LogControl"))
            {
                do 
                {
                    // �ж������Ƿ�ѡ��
                    bool bSCDIEDLNCheck_LogControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_LogControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Attr_R").etValue;
                    bool bSCDIEDLNCheck_LogControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_LogControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Attr_O").etValue;
                    bool bSCDIEDLNCheck_LogControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Child").bEnabled;
                    ErrorType etSCDIEDLNCheck_LogControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Child").etValue;
                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // LogControlԪ�غ���name, desc, datSet, intgPd, logName, logEna��reasonCode��7������
                    // LogControlԪ�غ�����Ԫ�أ�TrgOps����ѡ��Ԫ��
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_LogControl_Attr_R, 2);  //  desc="LogControlԪ�غ���name, logName��2������"
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_LogControl_Attr_O, 5);  //  desc="LogControlԪ�غ���desc, datSet, intgPd, logEna��reasonCode��5������"
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_LogControl_Child, 1); 

                    string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControlȱ��name����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_R, etSCDIEDLNCheck_LogControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControlȱ��desc����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_O, etSCDIEDLNCheck_LogControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"datSet"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControlȱ��datSet����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_O, etSCDIEDLNCheck_LogControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"intgPd"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControlȱ��intgPd����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_O, etSCDIEDLNCheck_LogControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"logName"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControlȱ��logName����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_R, etSCDIEDLNCheck_LogControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"logEna"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControlȱ��logEna����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_O, etSCDIEDLNCheck_LogControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"reasonCode"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControlȱ��reasonCode����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_O, etSCDIEDLNCheck_LogControl_Attr_O);
                    }

                    // child
                    if (m_vn->toElement(FIRST_CHILD, L"TrgOps"))
                    {
                        bool bSCDIEDLNCheck_TrgOps_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_TrgOps_Attr").bEnabled;
                        ErrorType etSCDIEDLNCheck_TrgOps_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_TrgOps_Attr").etValue;
                        // TrgOpsԪ�غ���dchg, qchg, dupd��period��4������
                        AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_TrgOps_Attr, 4);
                        // CheckTrgOpsAttr(bSCDIEDLNCheck_TrgOps_Attr);
                        if (!m_vn->hasAttr(L"dchg"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl��TrgOps��Ԫ��ȱ��dchg����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr);
                        }
                        if (!m_vn->hasAttr(L"qchg"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl��TrgOps��Ԫ��ȱ��qchg����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr);
                        }
                        if (!m_vn->hasAttr(L"dupd"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl��TrgOps��Ԫ��ȱ��dupd����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr);
                        }
                        if (!m_vn->hasAttr(L"period"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl��TrgOps��Ԫ��ȱ��period����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr);
                        }
                        m_vn->toElement(PARENT);
                    }
                    else
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControlȱ��TrgOps��Ԫ��", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Child, etSCDIEDLNCheck_LogControl_Child);
                    }

                    ControlBlockInfoCache CB;
                    CB.name = VtdGetAttributeValue(m_vn, L"name");
                    CB.dataSetName = VtdGetAttributeValue(m_vn, L"datSet");
                    CB.offset = GetOffsetOfVTD();

                    ControlBlockInfoMap::iterator it = ldCache.mapLogControl.find(CB.name);
                    if (it != ldCache.mapLogControl.end())
                    {
                        int row = pChecker->CalcRowNumberFromOffset(CB.offset);
                        string xml = pChecker->GetRowContentByOffset(CB.offset);
                        string msg = "��־���ƿ�����ظ�:IED name=%s LDevice inst=%s ControlBlock name=%s";
                        CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(CB.name).c_str());
                    } 
                    else
                    {
                        ldCache.mapLogControl.insert(make_pair(CB.name, CB));
                    }
                } while (m_vn->toElement(NEXT_SIBLING, L"LogControl"));
                m_vn->toElement(PARENT);
            }
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::CheckTrgOpsAttr(bool bSubItemChecked, ErrorType etValue)
{
    if (!m_vn->hasAttr(L"dchg"))
    {
        string msg = "TrgOpsԪ��ȱ��dchg����";
        SCDIEDLNCheck(msg, bSubItemChecked, etValue);
    }
    if (!m_vn->hasAttr(L"qchg"))
    {
        string msg = "TrgOpsԪ��ȱ��qchg����";
        SCDIEDLNCheck(msg, bSubItemChecked, etValue);
    }
    if (!m_vn->hasAttr(L"dupd"))
    {
        string msg = "TrgOpsԪ��ȱ��dupd����";
        SCDIEDLNCheck(msg, bSubItemChecked, etValue);
    }
    if (!m_vn->hasAttr(L"period"))
    {
        string msg = "TrgOpsԪ��ȱ��period����";
        SCDIEDLNCheck(msg, bSubItemChecked, etValue);
    }
    return true;
}

// ���SettingControl��SCLControl
bool CCacheMgr::GetSCLControlCache(LDCache &ldCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"LN0"))
        {
            if (m_vn->toElement(FIRST_CHILD, L"SCLControl"))
            {
                do 
                {
                    // �ж������Ƿ�ѡ��
                    bool bSCDIEDLNCheck_SCLControl_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SCLControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_SCLControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SCLControl_Attr_O").etValue;

                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // SCLControlԪ�غ���desc����
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SCLControl_Attr, 1); 

                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]SCLControlȱ��desc����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SCLControl_Attr, etSCDIEDLNCheck_SCLControl_Attr_O);
                    }
                    
                    /*ControlBlockInfoCache CB;
                    CB.name = VtdGetAttributeValue(m_vn, L"name");
                    CB.dataSetName = VtdGetAttributeValue(m_vn, L"datSet");
                    CB.offset = GetOffsetOfVTD();

                    ControlBlockInfoMap::iterator it = ldCache.mapLogControl.find(CB.name);
                    if (it != ldCache.mapLogControl.end())
                    {
                        int row = pChecker->CalcRowNumberFromOffset(CB.offset);
                        string xml = pChecker->GetRowContentByOffset(CB.offset);
                        string msg = "��־���ƿ�����ظ�:IED name=%s LDevice inst=%s ControlBlock name=%s";
                        CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(CB.name).c_str());
                    } 
                    else
                    {
                        ldCache.mapLogControl.insert(make_pair(CB.name, CB));
                    }*/
                } while (m_vn->toElement(NEXT_SIBLING, L"SCLControl"));
                m_vn->toElement(PARENT);
            }
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetSettingControlCache(LDCache &ldCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"LN0"))
        {
            if (m_vn->toElement(FIRST_CHILD, L"SettingControl"))
            {
                do 
                {
                    // �ж������Ƿ�ѡ��
                    bool bSCDIEDLNCheck_SettingControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SettingControl_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_SettingControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SettingControl_Attr_R").etValue;

                    bool bSCDIEDLNCheck_SettingControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SettingControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_SettingControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SettingControl_Attr_O").etValue;

                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // SettingControlԪ�غ���desc, numOfSGs��actSG��3������
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SettingControl_Attr_R, 1); 
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SettingControl_Attr_O, 2); 

                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]SettingControlȱ��desc����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SettingControl_Attr_R, etSCDIEDLNCheck_SettingControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"numOfSGs"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]SettingControlȱ��numOfSGs����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SettingControl_Attr_O, etSCDIEDLNCheck_SettingControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"actSG"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]SettingControlȱ��actSG����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SettingControl_Attr_O, etSCDIEDLNCheck_SettingControl_Attr_O);
                    }                    

                    //ControlBlockInfoCache CB;
                    //CB.name = VtdGetAttributeValue(m_vn, L"name");
                    //CB.dataSetName = VtdGetAttributeValue(m_vn, L"datSet");
                    //CB.offset = GetOffsetOfVTD();

                    //ControlBlockInfoMap::iterator it = ldCache.mapLogControl.find(CB.name);
                    //if (it != ldCache.mapLogControl.end())
                    //{
                    //    int row = pChecker->CalcRowNumberFromOffset(CB.offset);
                    //    string xml = pChecker->GetRowContentByOffset(CB.offset);
                    //    string msg = "��־���ƿ�����ظ�:IED name=%s LDevice inst=%s ControlBlock name=%s";
                    //    CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                    //        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(CB.name).c_str());
                    //} 
                    //else
                    //{
                    //    ldCache.mapLogControl.insert(make_pair(CB.name, CB));
                    //}
                } while (m_vn->toElement(NEXT_SIBLING, L"SettingControl"));
                m_vn->toElement(PARENT);
            }
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}


bool CCacheMgr::GetSampledValueControlCache(LDCache &ldCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"LN0"))
        {
            if (m_vn->toElement(FIRST_CHILD, L"SampledValueControl"))
            {
                do 
                {
                    // �ж������Ƿ�ѡ��
                    bool bSCDIEDLNCheck_SampledValueControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_SampledValueControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Attr_R").etValue;

                    bool bSCDIEDLNCheck_SampledValueControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_SampledValueControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Attr_O").etValue;

                    bool bSCDIEDLNCheck_SampledValueControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Child").bEnabled;
                    ErrorType etSCDIEDLNCheck_SampledValueControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Child").etValue;

                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // SampledValueControlԪ�غ���name, desc, datSet, confRev, smvID, multicast, smpRate��nofASDU��8������
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SampledValueControl_Attr_R, 4); //  desc="SampledValueControlԪ�غ���name, smvID, smpRate��nofASDU��4������"
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SampledValueControl_Attr_O, 4); 
                    // SampledValueControlԪ�غ�����Ԫ��SmvOpts����ֵѡ��Ԫ�ء���Ԫ��Ϊ��ѡ
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SampledValueControl_Child, 1); 

                    string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControlȱ��name����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_R, etSCDIEDLNCheck_SampledValueControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControlȱ��desc����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_O, etSCDIEDLNCheck_SampledValueControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"datSet"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControlȱ��datSet����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_O, etSCDIEDLNCheck_SampledValueControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"confRev"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControlȱ��confRev����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_O, etSCDIEDLNCheck_SampledValueControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"smvID"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControlȱ��smvID����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_R, etSCDIEDLNCheck_SampledValueControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"multicast"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControlȱ��multicast����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_O, etSCDIEDLNCheck_SampledValueControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"smpRate"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControlȱ��smpRate����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_R, etSCDIEDLNCheck_SampledValueControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"nofASDU"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControlȱ��nofASDU����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_R, etSCDIEDLNCheck_SampledValueControl_Attr_R);
                    }
                    
                    if (m_vn->toElement(FIRST_CHILD, L"SmvOpts"))
                    {
                        bool bSCDIEDLNCheck_SmvOpts_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SmvOpts_Attr_O").bEnabled;
                        ErrorType etSCDIEDLNCheck_SmvOpts_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SmvOpts_Attr_O").etValue;

                        // SmvOptsԪ�غ���refreshTime, sampleSynchronized, sampleRate, security��dataRef��5������
                         AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SmvOpts_Attr, 5); 
                         if (!m_vn->hasAttr(L"refreshTime"))
                         {
                             string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl��SmvOpts��Ԫ��ȱ��refreshTime����", 
                                 WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                             SCDIEDLNCheck(msg, bSCDIEDLNCheck_SmvOpts_Attr, etSCDIEDLNCheck_SmvOpts_Attr_O);
                         }
                         if (!m_vn->hasAttr(L"sampleSynchronized"))
                         {
                             string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl��SmvOpts��Ԫ��ȱ��sampleSynchronized����", 
                                 WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                             SCDIEDLNCheck(msg, bSCDIEDLNCheck_SmvOpts_Attr, etSCDIEDLNCheck_SmvOpts_Attr_O);
                         }
                         if (!m_vn->hasAttr(L"sampleRate"))
                         {
                             string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl��SmvOpts��Ԫ��ȱ��sampleRate����", 
                                 WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                             SCDIEDLNCheck(msg, bSCDIEDLNCheck_SmvOpts_Attr, etSCDIEDLNCheck_SmvOpts_Attr_O);
                         }
                         if (!m_vn->hasAttr(L"security"))
                         {
                             string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl��SmvOpts��Ԫ��ȱ��security����", 
                                 WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                             SCDIEDLNCheck(msg, bSCDIEDLNCheck_SmvOpts_Attr, etSCDIEDLNCheck_SmvOpts_Attr_O);
                         }
                         if (!m_vn->hasAttr(L"dataRef"))
                         {
                             string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl��SmvOpts��Ԫ��ȱ��dataRef����", 
                                 WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                             SCDIEDLNCheck(msg, bSCDIEDLNCheck_SmvOpts_Attr, etSCDIEDLNCheck_SmvOpts_Attr_O);
                         }

                        m_vn->toElement(PARENT);
                    }
                    else
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControlȱ��SmvOpts��Ԫ��", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Child, etSCDIEDLNCheck_SampledValueControl_Child);
                    }

                    ControlBlockInfoCache smvCB;
                    smvCB.name = VtdGetAttributeValue(m_vn, L"name");
                    smvCB.appID = VtdGetAttributeValue(m_vn, L"smvID");
                    smvCB.dataSetName = VtdGetAttributeValue(m_vn, L"datSet");
                    smvCB.offset = GetOffsetOfVTD();

                    ControlIdUniqueCheck(smvCB.appID);

                    ControlBlockInfoMap::iterator it = ldCache.mapSampledValueControl.find(smvCB.name);
                    if (it != ldCache.mapSampledValueControl.end())
                    {
                        int row = pChecker->CalcRowNumberFromOffset(smvCB.offset);
                        string xml = pChecker->GetRowContentByOffset(smvCB.offset);
                        string msg = "SampledValueControl�����ظ�:IED name=%s LDevice inst=%s ControlBlock name=%s";
                        CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(smvCB.name).c_str());
                    } 
                    else
                    {
                        ldCache.mapSampledValueControl.insert(make_pair(smvCB.name, smvCB));
                    }
                } while (m_vn->toElement(NEXT_SIBLING, L"SampledValueControl"));
                m_vn->toElement(PARENT);
            }
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
    	return false;
    }
    return true;
}

bool CCacheMgr::GetInputsCache(LDCache &ldCache)
{
    if (!IsNeededCacheMarkExists("Inputs"))
    {
        return true;
    }
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"LN0"))
        {
            if (m_vn->toElement(FIRST_CHILD, L"Inputs"))
            {
                
                // �ж������Ƿ�ѡ��
                bool bSCDIEDLNCheck_Inputs_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Inputs_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_Inputs_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Inputs_Attr_O").etValue;

                bool bSCDIEDLNCheck_Inputs_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Inputs_Child").bEnabled;   
                ErrorType etSCDIEDLNCheck_Inputs_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Inputs_Child").etValue;

                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                // InputsԪ�غ���desc����
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_Inputs_Attr, 1);
                // InputsԪ�غ�����Ԫ��ExRef�ⲿ��ϢԪ��
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_Inputs_Child, 1);

                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]InputsԪ��ȱ��desc����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_Inputs_Attr, etSCDIEDLNCheck_Inputs_Attr_O);
                }

                if (m_vn->toElement(FIRST_CHILD, L"ExtRef"))
                {
                    do 
                    {
                        bool bSCDIEDLNCheck_ExtRef_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ExtRef_Attr_O").bEnabled;
                        ErrorType etSCDIEDLNCheck_ExtRef_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ExtRef_Attr_O").etValue;

                        // ExtRefԪ�غ���iedName, IdInst, prefix, InClass, InInst, doName, daName��intAddr��8�����ԡ�
                        AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_ExtRef_Attr, 8);
                        

                        InputsCache inputCache;
                        wstring lnClass, lnInst, prefix, doName, daName;
                        inputCache.iedName = VtdGetAttributeValue(m_vn, L"iedName");
                        inputCache.ldInst = VtdGetAttributeValue(m_vn, L"ldInst");
                        lnClass = VtdGetAttributeValue(m_vn, L"lnClass");
                        lnInst = VtdGetAttributeValue(m_vn, L"lnInst");
                        prefix = VtdGetAttributeValue(m_vn, L"prefix");
                        doName = VtdGetAttributeValue(m_vn, L"doName");
                        daName = VtdGetAttributeValue(m_vn, L"daName");
                        int offset = GetOffsetOfVTD();
                        inputCache.offset = offset;

                        inputCache.outSignal = inputCache.ldInst + L"/" + prefix + lnClass + lnInst + 
                            L"." + doName;
                        if (daName.size() > 0)
                        {
                            inputCache.outSignal = inputCache.outSignal + L"." + daName;
                        }

                        wstring intAddr = VtdGetAttributeValue(m_vn, L"intAddr");
                        inputCache.intAddr = intAddr;
                        // ����̵�ַ�а�������˿���Ϣ�����
                        int pos = intAddr.find(L":");
                        if (pos != wstring::npos)
                        {
                            inputCache.Port = intAddr.substr(0, pos);
                            inputCache.inSignal = intAddr.substr(pos+1);
                        }
                        else
                        {
                            inputCache.inSignal = intAddr;
                        }
                        // InputsFormatCheck(ldCache, intAddr);
                        IntAddrUniqueCheck(ldCache, inputCache.inSignal);
                        // ����ṹ�б���ͳ�����·
                        wstring outKey = inputCache.iedName + L"." + inputCache.ldInst + L"/" + prefix + lnClass + lnInst + 
                            L"." + doName;
                        if (daName.size() > 0)
                        {
                            outKey = outKey + L"." + daName;
                        }
                        wstring inKey = ldCache.iedName + L"." + inputCache.inSignal;
                        m_SCDCache.vlinkByOutSignal[outKey].insert(inKey);
                        m_SCDCache.vlinkByInSignal[inKey] = outKey;

                        CreatAssoIEDMap(ldCache.iedName, inputCache.iedName, inputCache.offset);

                        // �����Լ��
                        if (!m_vn->hasAttr(L"iedName"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRefԪ��ȱ��iedName����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"ldInst"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRefԪ��ȱ��ldInst����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"prefix"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRefԪ��ȱ��prefix����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"lnClass"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRefԪ��ȱ��lnClass����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"lnInst"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRefԪ��ȱ��lnInst����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"doName"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRefԪ��ȱ��doName����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"daName"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRefԪ��ȱ��daName����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"intAddr"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRefԪ��ȱ��intAddr����", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }

                        wstring key = inputCache.iedName + inputCache.outSignal + inputCache.inSignal;
                        InputsMap::iterator it = ldCache.mapInputs.find(key);
                        if (it != ldCache.mapInputs.end())
                        {
                            int row = pChecker->CalcRowNumberFromOffset(offset);
                            string xml = pChecker->GetRowContentByOffset(offset);
                            string msg = "���·�����ظ�:IED name=%s LDevice inst=%s Inputs �ڲ���ַintAddr=%s �ⲿ����ExtRef=%s";
                            CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str(), WtoA(inputCache.outSignal).c_str());
                        } 
                        else
                        {
                            ldCache.mapInputs.insert(make_pair(key, inputCache));
                        }

                    } while (m_vn->toElement(NEXT_SIBLING, L"ExtRef"));
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]InputsԪ��ȱ��ExtRef��Ԫ��",
                         WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_Inputs_Child, etSCDIEDLNCheck_Inputs_Child);
                }
                m_vn->toElement(PARENT);
            }
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::CreatAssoIEDMap(wstring &in, wstring &out, int offset)
{
    if (IsNeededCacheMarkExists("SwitchBay"))
    {

        m_SCDCache.mapAssoIED[in].insert(make_pair(out, offset));
        // m_SCDCache.mapAssoIED[out].insert(in); ��ֹ�ظ���������Ӵ���
    }
    return true;
}

bool CCacheMgr::GetLogicNodeInstanceCache(LDCache &ldCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"LN0"))
        {
            GetDOICache(ldCache);
            m_vn->toElement(PARENT);
        }

        if (m_vn->toElement(FIRST_CHILD, L"LN"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bSCDIEDLNCheck_LN_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN_Attr_R").bEnabled;
                ErrorType etSCDIEDLNCheck_LN_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN_Attr_R").etValue;

                bool bSCDIEDLNCheck_LN_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_LN_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN_Attr_O").etValue;

                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                // LNԪ�غ���InClass, InType, inst, desc��prefix��5������
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_LN_Attr_R, 3); //  desc="LNԪ�غ���InClass, InType, inst��3������"
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_LN_Attr_O, 2); 

                wstring lnClass = VtdGetAttributeValue(m_vn, L"lnClass");
                wstring prefix = VtdGetAttributeValue(m_vn, L"prefix");
                wstring inst = VtdGetAttributeValue(m_vn, L"inst");


                string key = WtoA(prefix + lnClass +inst);
                if (!m_vn->hasAttr(L"lnClass"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s]LNԪ��ȱ��lnClass����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), key.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN_Attr_R, etSCDIEDLNCheck_LN_Attr_R);
                }
                if (!m_vn->hasAttr(L"lnType"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s]LNԪ��ȱ��lnType����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), key.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN_Attr_R, etSCDIEDLNCheck_LN_Attr_R);
                }
                if (!m_vn->hasAttr(L"inst"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s]LNԪ��ȱ��inst����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), key.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN_Attr_R, etSCDIEDLNCheck_LN_Attr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s]LNԪ��ȱ��desc����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), key.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN_Attr_O, etSCDIEDLNCheck_LN_Attr_O);
                }
                if (!m_vn->hasAttr(L"prefix"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s]LNԪ��ȱ��prefix����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), key.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN_Attr_O, etSCDIEDLNCheck_LN_Attr_O);
                }

                GetDOICache(ldCache);
            } while (m_vn->toElement(NEXT_SIBLING, L"LN"));
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetDOICache(LDCache &ldCache)
{
    try
    {
        wstring lnType = VtdGetAttributeValue(m_vn, L"lnType");
        wstring desc = VtdGetAttributeValue(m_vn, L"desc");
        wstring lnClass = VtdGetAttributeValue(m_vn, L"lnClass");
        wstring prefix = VtdGetAttributeValue(m_vn, L"prefix");
        wstring inst = VtdGetAttributeValue(m_vn, L"inst");

        LogicalNodeCache lnNode;
        lnNode.key = prefix + lnClass +inst;
        lnNode.lnType = lnType;
        lnNode.offset = GetOffsetOfVTD();

        LogicalNodeMap::iterator itLn = ldCache.mapLogicalNode.find(lnNode.key);
        if (itLn != ldCache.mapLogicalNode.end())
        {
            int row = pChecker->CalcRowNumberFromOffset(lnNode.offset);
            string xml = pChecker->GetRowContentByOffset(lnNode.offset);
            string msg = "��ǰ�ڵ���Lnʵ����Ψһ:IED name=%s LDevice inst=%s LN=%s";
            CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(lnNode.key).c_str());
        }
        else
        {
            ldCache.mapLogicalNode.insert(make_pair(lnNode.key, lnNode));
        }

        if (!IsNeededCacheMarkExists("LogicNodeInstance"))
        {
            return true;
        }
        if (m_vn->toElement(FIRST_CHILD, L"DOI"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bSCDIEDLNCheck_DOI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Attr_R").bEnabled;
                ErrorType etSCDIEDLNCheck_DOI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Attr_R").etValue;

                bool bSCDIEDLNCheck_DOI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_DOI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Attr_O").etValue;

                bool bSCDIEDLNCheck_DOI_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Child_O").bEnabled;
                ErrorType etSCDIEDLNCheck_DOI_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Child_O").etValue;

                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                // DOIԪ�غ���desc, name, ix��accessControl��4������
                // DOIԪ�غ�����Ԫ�أ�DAI��������ʵ��Ԫ�ء�SDI������ʵ��Ԫ�ء����Ǳ�ѡԪ��
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DOI_Attr_R, 1); //  desc="DOIԪ�غ�name��1������"
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DOI_Attr_O, 3);
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DOI_Child, 2);

                string doiName = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOIԪ��ȱ��desc����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(lnNode.key).c_str(), doiName.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DOI_Attr_O, etSCDIEDLNCheck_DOI_Attr_O);
                }
                if (!m_vn->hasAttr(L"name"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOIԪ��ȱ��name����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(lnNode.key).c_str(), doiName.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DOI_Attr_R, etSCDIEDLNCheck_DOI_Attr_R);
                }
                if (!m_vn->hasAttr(L"ix"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOIԪ��ȱ��ix����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(lnNode.key).c_str(), doiName.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DOI_Attr_O, etSCDIEDLNCheck_DOI_Attr_O);
                }
                if (!m_vn->hasAttr(L"accessControl"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOIԪ��ȱ��accessControl����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(lnNode.key).c_str(), doiName.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DOI_Attr_O, etSCDIEDLNCheck_DOI_Attr_O);
                }

                // child
                if (m_vn->toElement(FIRST_CHILD, L"DAI"))
                {
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOIԪ��ȱ��DAI��Ԫ��", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(lnNode.key).c_str(), doiName.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DOI_Child, etSCDIEDLNCheck_DOI_Child_O);
                }
                if (m_vn->toElement(FIRST_CHILD, L"SDI"))
                {
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOIԪ��ȱ��SDI��Ԫ��", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(lnNode.key).c_str(), doiName.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DOI_Child, etSCDIEDLNCheck_DOI_Child_O);
                }

                LogicalNodesInstanceCache doiInstanceCache;
                doiInstanceCache.DOI = VtdGetAttributeValue(m_vn, L"name");
                doiInstanceCache.DOIDesc = VtdGetAttributeValue(m_vn, L"desc");
                doiInstanceCache.LN = prefix + lnClass + inst;
                doiInstanceCache.LNType = lnType;
                doiInstanceCache.LNDesc = desc;
                doiInstanceCache.offset = GetOffsetOfVTD();

                doiInstanceCache.key = ldCache.inst + L"/" + doiInstanceCache.LN + L"." + 
                    doiInstanceCache.DOI ;
                LogicalNodesInstancesMap::iterator it = ldCache.mapSignalInstance.find(doiInstanceCache.key);
                if (it != ldCache.mapSignalInstance.end())
                {
                    int row = pChecker->CalcRowNumberFromOffset(doiInstanceCache.offset);
                    string xml = pChecker->GetRowContentByOffset(doiInstanceCache.offset);
                    string msg = "��ǰ�ڵ���DOIʵ����Ψһ:IED name=%s LDevice inst=%s LN=%s DOI name=%s";
                    CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(doiInstanceCache.LN).c_str(), WtoA(doiInstanceCache.DOI).c_str());
                }
                else
                {
                    ldCache.mapSignalInstance.insert(make_pair(doiInstanceCache.key, doiInstanceCache));
                }

                GetSDICache(ldCache, doiInstanceCache);
                GetDAICache(ldCache, doiInstanceCache);
            } while (m_vn->toElement(NEXT_SIBLING, L"DOI"));
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CCacheMgr::GetDAICache(LDCache &ldCache, LogicalNodesInstanceCache &doiInstanceCache)
{
    try
    {
        if(m_vn->toElement(FIRST_CHILD, L"DAI"))
        {
            do
            {
                // �ж������Ƿ�ѡ��
                bool bSCDIEDLNCheck_DAI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Attr_R").bEnabled;
                ErrorType etSCDIEDLNCheck_DAI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Attr_R").etValue;

                bool bSCDIEDLNCheck_DAI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_DAI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Attr_O").etValue;

                bool bSCDIEDLNCheck_DAI_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Child_O").bEnabled;
                ErrorType etSCDIEDLNCheck_DAI_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Child_O").etValue;

                bool bSCDIEDLNCheck_Val_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Val_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_Val_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Val_Attr_O").etValue;

                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                // DAIԪ�غ���desc, name, sAddr, valKind��ix��5������
                // DAIԪ�غ�����Ԫ�أ�Val����ֵԪ��
                
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DAI_Attr_R, 1);
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DAI_Attr_O, 4);
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DAI_Child, 1);
                
                LogicalNodesInstanceCache daiCache;
                daiCache.LN = doiInstanceCache.LN;
                daiCache.LNType = doiInstanceCache.LNType;
                daiCache.LNDesc = doiInstanceCache.LNDesc;
                daiCache.DOI = doiInstanceCache.DOI;
                daiCache.DOIDesc = doiInstanceCache.DOIDesc;
                daiCache.DAI = doiInstanceCache.DAI;
                daiCache.dUVal = doiInstanceCache.dUVal;
                daiCache.offset = GetOffsetOfVTD();

                wstring daname = VtdGetAttributeValue(m_vn, L"name");
                if (daiCache.DAI.empty())
                {
                    daiCache.DAI = daname;
                }
                else
                {
                    daiCache.DAI = daiCache.DAI + L"." + daname;
                }


                daiCache.key = doiInstanceCache.key + L"." + daname;
                if(daname.compare(L"dU") == 0)
                {
                    if(m_vn->toElement(FIRST_CHILD, L"Val"))
                    {
                        daiCache.dUVal = VtdGetTextValue(m_vn);
                        m_vn->toElement(PARENT);
                    }
                }

                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAIȱ��desc����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Attr_O, etSCDIEDLNCheck_DAI_Attr_O);
                }
                if (!m_vn->hasAttr(L"name"))
                {
                    string msg =  FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAIȱ��name����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Attr_R, etSCDIEDLNCheck_DAI_Attr_R);
                }
                if (!m_vn->hasAttr(L"sAddr"))
                {
                    string msg =  FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAIȱ��sAddr����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Attr_O, etSCDIEDLNCheck_DAI_Attr_O);
                }
                if (!m_vn->hasAttr(L"valKind"))
                {
                    string msg =  FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAIȱ��valKind����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Attr_O, etSCDIEDLNCheck_DAI_Attr_O);
                }
                if (!m_vn->hasAttr(L"ix"))
                {
                    string msg =  FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAIȱ��ix����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Attr_O, etSCDIEDLNCheck_DAI_Attr_O);
                }

                // child
                if (m_vn->toElement(FIRST_CHILD, L"Val"))
                {
                    // ValԪ�غ���sGroup����
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_Val_Attr, 1);
                    if (!m_vn->hasAttr(L"sGroup"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAI��Ԫ��Valȱ��sGroup����", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_Val_Attr, etSCDIEDLNCheck_Val_Attr_O);
                    }
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAIȱ��Val��Ԫ��", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Child, etSCDIEDLNCheck_DAI_Child_O);
                }

                LogicalNodesInstancesMap::iterator it = ldCache.mapSignalInstance.find(daiCache.key);
                if (it != ldCache.mapSignalInstance.end())
                {
                    int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
                    string xml = pChecker->GetRowContentByRowNumber(row);
                    string msg = "��ǰ�ڵ���DAIʵ����Ψһ:IED name=%s LDevice inst=%s LN=%s DOI name=%s DAI name=%s";
                    CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(doiInstanceCache.LN).c_str(), WtoA(doiInstanceCache.DOI).c_str(), WtoA(daname).c_str());
                }
                else
                {
                    ldCache.mapSignalInstance.insert(make_pair(daiCache.key, daiCache));
                }

            }while(m_vn->toElement(NEXT_SIBLING, L"DAI"));
            m_vn->toElement(PARENT); //DAI end
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CCacheMgr::GetSDICache(LDCache &ldCache, LogicalNodesInstanceCache &doiInstanceCache)
{
    try
    {
        if(m_vn->toElement(FIRST_CHILD, L"SDI"))
        {
            do
            {
                // �ж������Ƿ�ѡ��
                bool bSCDIEDLNCheck_SDI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Attr_R").bEnabled;
                ErrorType etSCDIEDLNCheck_SDI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Attr_R").etValue;

                bool bSCDIEDLNCheck_SDI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_SDI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Attr_O").etValue;

                bool bSCDIEDLNCheck_SDI_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Child_O").bEnabled;
                ErrorType etSCDIEDLNCheck_SDI_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Child_O").etValue;

                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                // SDIԪ�غ���desc, name��ix��3������
                // SDIԪ�غ�����Ԫ�أ�DAI��������ʵ��Ԫ�ء�SDI������ʵ��Ԫ�ء����Ǳ�ѡԪ��
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SDI_Attr_R, 1);
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SDI_Attr_O, 2);
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SDI_Child, 2);
                
                LogicalNodesInstanceCache sdiInstanceCache;
                sdiInstanceCache.LN = doiInstanceCache.LN;
                sdiInstanceCache.LNType = doiInstanceCache.LNType;
                sdiInstanceCache.LNDesc = doiInstanceCache.LNDesc;
                sdiInstanceCache.DOI = doiInstanceCache.DOI;
                sdiInstanceCache.DOIDesc = doiInstanceCache.DOIDesc;
                sdiInstanceCache.DAI = doiInstanceCache.DAI;
                sdiInstanceCache.dUVal = doiInstanceCache.dUVal;
                sdiInstanceCache.offset = GetOffsetOfVTD();
                wstring sdiDesc = VtdGetAttributeValue(m_vn, L"desc");
                sdiInstanceCache.DOIDesc = sdiDesc;

                wstring sdiname = VtdGetAttributeValue(m_vn, L"name");
                if (sdiInstanceCache.DAI.empty())
                {
                    sdiInstanceCache.DAI = sdiname;
                }
                else
                {
                    sdiInstanceCache.DAI = sdiInstanceCache.DAI + L"." + sdiname;
                }
                sdiInstanceCache.key = doiInstanceCache.key + L"." + sdiname;

                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SDI name=%s]SDIԪ��ȱ��desc����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(sdiInstanceCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_SDI_Attr_O, etSCDIEDLNCheck_SDI_Attr_O);
                }
                if (!m_vn->hasAttr(L"name"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SDI name=%s]SDIԪ��ȱ��name����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(sdiInstanceCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_SDI_Attr_R, etSCDIEDLNCheck_SDI_Attr_R);
                }
                if (!m_vn->hasAttr(L"ix"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SDI name=%s]SDIԪ��ȱ��ix����", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(sdiInstanceCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_SDI_Attr_O, etSCDIEDLNCheck_SDI_Attr_O);
                }
                // child
                if (m_vn->toElement(FIRST_CHILD, L"DAI"))
                {
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SDI name=%s]SDIԪ��ȱ��DAI��Ԫ��", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(sdiInstanceCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_SDI_Child, etSCDIEDLNCheck_SDI_Child_O);
                }
                if (m_vn->toElement(FIRST_CHILD, L"SDI"))
                {
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SDI name=%s]SDIԪ��ȱ��SDI��Ԫ��", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(sdiInstanceCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_SDI_Child, etSCDIEDLNCheck_SDI_Child_O);
                }

                LogicalNodesInstancesMap::iterator it = ldCache.mapSignalInstance.find(sdiInstanceCache.key);
                if (it != ldCache.mapSignalInstance.end())
                {
                    int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
                    string xml = WtoA(GetXMLContent());
                    string msg = "��ǰ�ڵ���SDIʵ����Ψһ:IED name=%s LDevice inst=%s LN=%s DOI name=%s SDI name=%s";
                    CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(), 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(doiInstanceCache.LN).c_str(), WtoA(doiInstanceCache.DOI).c_str(), WtoA(doiInstanceCache.DAI).c_str());
                }
                else
                {
                    ldCache.mapSignalInstance.insert(make_pair(sdiInstanceCache.key, sdiInstanceCache));
                }

                GetSDICache(ldCache, sdiInstanceCache);
                GetDAICache(ldCache, sdiInstanceCache);
            }while(m_vn->toElement(NEXT_SIBLING, L"SDI"));
            m_vn->toElement(PARENT); //SDI end
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

// Header Cache
bool CCacheMgr::GetHeaderCache()
{
    try
    {
        m_vn->toElement(ROOT);
        if (m_vn->toElement(FIRST_CHILD, L"Header"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Attr_R").bEnabled;
                ErrorType etSCDHeaderCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Attr_R").etValue;

                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Attr_O").bEnabled;
                ErrorType etSCDHeaderCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Attr_O").etValue;

                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Child").bEnabled;
                ErrorType etSCDHeaderCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Child").etValue;

                bool bHistoryChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_History_Child").bEnabled; 
                ErrorType etSCDHeaderCheck_History_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_History_Child").etValue;

                bool bParentChecked = CSCDHeaderCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 1); // HeaderԪ���Ƿ����ID1������
                AddDueElementInfo(bParentChecked, bAttr_O, 4); // HeaderԪ���Ƿ����Version��reversion��toolID��namestructure 4������
                AddDueElementInfo(bParentChecked, bChild, 1); // HeaderԪ���Ƿ����ٰ���History��Ԫ��

                if (m_vn->hasAttr(L"id"))
                {
                    m_SCDCache.header.id = VtdGetAttributeValue(m_vn, L"id");
                } 
                else
                {
                    string msg = "HeaderԪ��ȱ��id����";
                    SCDHeaderCheck(msg, bAttr_R, etSCDHeaderCheck_Attr_R);
                }
                if (m_vn->hasAttr(L"version"))
                {
                    m_SCDCache.header.version = VtdGetAttributeValue(m_vn, L"version");
                } 
                else
                {
                    string msg = "HeaderԪ��ȱ��version����";
                    SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Attr_O);
                }
                if (m_vn->hasAttr(L"revision"))
                {
                    m_SCDCache.header.reversion = VtdGetAttributeValue(m_vn, L"revision");
                } 
                else
                {
                    string msg = "HeaderԪ��ȱ��revision����";
                    SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Attr_O);
                }
                if (m_vn->hasAttr(L"toolID"))
                {
                    m_SCDCache.header.toolID = VtdGetAttributeValue(m_vn, L"toolID");
                } 
                else
                {
                    string msg = "HeaderԪ��ȱ��toolID����";
                    SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Attr_O);
                }
                if (m_vn->hasAttr(L"nameStructure"))
                {
                    m_SCDCache.header.namestructure = VtdGetAttributeValue(m_vn, L"nameStructure");
                } 
                else
                {
                    string msg = "HeaderԪ��ȱ��nameStructure����";
                    SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Attr_O);
                }

                // ��Ԫ��
                bool bHaveHistory = false;

                if (m_vn->toElement(FIRST_CHILD))
                {
                    do 
                    {
                        wstring tag = VtdGetElementName(m_vn);
                        if (tag == L"History")
                        {
                            bHaveHistory = true;
                            AddDueElementInfo(bParentChecked, bHistoryChild, 1); // History�Ƿ���Hitem��Ԫ��

                            // �Ƿ���Hitem
                            bool bHaveHitem = false;
                            if (m_vn->toElement(FIRST_CHILD))
                            {
                                do 
                                {
                                    wstring tagName = VtdGetElementName(m_vn);
                                    if (tagName == L"Hitem")
                                    {
                                        bHaveHitem = true;
                                        GetHitemCache(m_SCDCache.header.history);
                                    }
                                } while (m_vn->toElement(NEXT_SIBLING));
                                m_vn->toElement(PARENT);
                            }
                            if (!bHaveHitem)
                            {
                                string msg = "HistoryԪ����ȱ��Hitem��Ԫ��";
                                SCDHeaderCheck(msg, bHistoryChild, etSCDHeaderCheck_History_Child);
                            }
                        }
                        else
                        {
                            //TODO
                        }

                    } while (m_vn->toElement(NEXT_SIBLING));
                    // ���ظ�Ԫ��
                    m_vn->toElement(PARENT);
                }
                if (!bHaveHistory)
                {
                    string msg = "HeaderԪ����ȱ��History��Ԫ��";
                    SCDHeaderCheck(msg, bChild, etSCDHeaderCheck_Child);
                }

            } while (m_vn->toElement(NEXT_SIBLING, L"Header"));
        }
    }
    catch (...)
    {
    	return false;
    }
    return true;
}

 bool CCacheMgr::GetHitemCache(HitemVector &vec)
 {
     try
     {
          // �ж������Ƿ�ѡ��
         bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Hitem_Attr_R").bEnabled;
         ErrorType etSCDHeaderCheck_Hitem_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Hitem_Attr_R").etValue;

         bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Hitem_Attr_O").bEnabled;
         ErrorType etSCDHeaderCheck_Hitem_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Hitem_Attr_O").etValue;

         bool bParentChecked = CSCDHeaderCheck::GetInstance()->IsEnabled();
         AddDueElementInfo(bParentChecked, bAttr_R, 3); // ��Hitem���Ƿ���� ��Version������reversion������when��3������
         AddDueElementInfo(bParentChecked, bAttr_O, 3); // ��Hitem���Ƿ���� ��who������what������why��3������
         
         HitemCache hitem;
         if (m_vn->hasAttr(L"version"))
         {
             hitem.version = VtdGetAttributeValue(m_vn, L"version");
         } 
         else
         {
             string msg = "HitemԪ��ȱ��version����";
             SCDHeaderCheck(msg, bAttr_R, etSCDHeaderCheck_Hitem_Attr_R);
         }

         if (m_vn->hasAttr(L"revision"))
         {
             hitem.reversion = VtdGetAttributeValue(m_vn, L"revision");
         } 
         else
         {
             string msg = "HitemԪ��ȱ��revision����";
             SCDHeaderCheck(msg, bAttr_R, etSCDHeaderCheck_Hitem_Attr_R);
         }

         if (m_vn->hasAttr(L"when"))
         {
             hitem.when = VtdGetAttributeValue(m_vn, L"when");
         } 
         else
         {
             string msg = "HitemԪ��ȱ��when����";
             SCDHeaderCheck(msg, bAttr_R, etSCDHeaderCheck_Hitem_Attr_R);
         }

         if (m_vn->hasAttr(L"who"))
         {
             hitem.who = VtdGetAttributeValue(m_vn, L"who");
         } 
         else
         {
             string msg = "HitemԪ��ȱ��who����";
             SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Hitem_Attr_O);
         }

         if (m_vn->hasAttr(L"what"))
         {
             hitem.what = VtdGetAttributeValue(m_vn, L"what");
         } 
         else
         {
             string msg = "HitemԪ��ȱ��what����";
             SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Hitem_Attr_O);
         }

         if (m_vn->hasAttr(L"why"))
         {
             hitem.why = VtdGetAttributeValue(m_vn, L"why");
         } 
         else
         {
             string msg = "HitemԪ��ȱ��why����";
             SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Hitem_Attr_O);
         }

         vec.push_back(hitem);
     }
     catch (...)
     {
         return false;     	
     }
     return true;
 }

// Communication Cache
bool CCacheMgr::GetCommunicationCache()
{
    try
    {
        m_vn->toElement(ROOT);
        if (m_vn->toElement(FIRST_CHILD, L"Communication"))
        {
            // �ж������Ƿ�ѡ��
            bool bAttr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDCommunicationCheck_Attr_O").bEnabled;
            ErrorType etSCDCommunicationCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDCommunicationCheck_Attr_O").etValue;
            bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDCommunicationCheck_Child").bEnabled;
            ErrorType etSCDCommunicationCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDCommunicationCheck_Child").etValue;
            bool bParentChecked = CSCDCommunicationCheck::GetInstance()->IsEnabled();
            AddDueElementInfo(bParentChecked, bAttr, 1); // CommunicationԪ���Ƿ���desc ����
            AddDueElementInfo(bParentChecked, bChild, 1); // CommunicationԪ���Ƿ���һ����ѡ����Ԫ��SubNetwork����

            if (!m_vn->hasAttr(L"desc"))
            {
                string msg = "CommunicationԪ��ȱ��desc����";
                SCDCommunicationCheck(msg, bAttr, etSCDCommunicationCheck_Attr_O);
            } 

            bool bHaveSubNetWork = false;
            if (m_vn->toElement(FIRST_CHILD))
            {
                do 
                {
                    wstring tag = VtdGetElementName(m_vn);
                    if (tag == L"SubNetwork")
                    {
                        bHaveSubNetWork = true;
                        GetSubnetWorkCache();
                    }

                } while (m_vn->toElement(NEXT_SIBLING));

                m_vn->toElement(PARENT);
            }

            if (!bHaveSubNetWork)
            {
                string msg = "CommunicationԪ��ȱ��SubNetwork��Ԫ��";
                SCDCommunicationCheck(msg, bChild, etSCDCommunicationCheck_Child);
            }
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetSubnetWorkCache()
{
    try
    {
        // �ж������Ƿ�ѡ��
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSubNetWorkCheck_Attr_R").bEnabled;
        ErrorType etSCDSubNetWorkCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSubNetWorkCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSubNetWorkCheck_Attr_O").bEnabled;
        ErrorType etSCDSubNetWorkCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSubNetWorkCheck_Attr_O").etValue;

        bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSubNetWorkCheck_Child").bEnabled;
        ErrorType etSCDSubNetWorkCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSubNetWorkCheck_Child").etValue;

        bool bBitRate_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSubNetWorkCheck_BitRate_Attr_R").bEnabled;
        ErrorType etSCDSubNetWorkCheck_BitRate_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSubNetWorkCheck_BitRate_Attr_R").etValue;

        bool bBitRate_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSubNetWorkCheck_BitRate_Attr_O").bEnabled;
        ErrorType etSCDSubNetWorkCheck_BitRate_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSubNetWorkCheck_BitRate_Attr_O").etValue;

        bool bParentChecked = CSCDSubNetWorkCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 1); // SubNetworkԪ���Ƿ���name��1������
        AddDueElementInfo(bParentChecked, bAttr_O, 2); // SubNetworkԪ���Ƿ���desc��type ��2������
        AddDueElementInfo(bParentChecked, bChild, 1); // SubNetworkԪ�غ�����Ԫ�� BitRate�����ʡ�ConnectedAP���ӷ��ʵ㡣����ConnectedAPΪ��ѡ��
        
        SubNetWorkCache subNetCache;
        if (m_vn->hasAttr(L"name"))
        {
            subNetCache.subNetWorkName = VtdGetAttributeValue(m_vn, L"name");
        } 
        else
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWorkԪ��ȱ��name����", WtoA(subNetCache.subNetWorkName).c_str());
            SCDSubNetWorkCheck(msg, bAttr_R, etSCDSubNetWorkCheck_Attr_R);
        }
        long offset = GetOffsetOfVTD();
        
        if (!m_vn->hasAttr(L"desc"))
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWorkԪ��ȱ��desc����", WtoA(subNetCache.subNetWorkName).c_str());
            SCDSubNetWorkCheck(msg, bAttr_O, etSCDSubNetWorkCheck_Attr_O);
        }
        if (!m_vn->hasAttr(L"type"))
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWorkԪ��ȱ��type����", WtoA(subNetCache.subNetWorkName).c_str());
            SCDSubNetWorkCheck(msg, bAttr_O, etSCDSubNetWorkCheck_Attr_O);
        } 
        SubFormatCheck(subNetCache.subNetWorkName);
        
        // �Ƿ���ConnectedAP
        bool bHaveConnectedAP = false;
        if (m_vn->toElement(FIRST_CHILD))
        {
            do 
            {
                wstring tag = VtdGetElementName(m_vn);
                if (tag == L"ConnectedAP")
                {
                    bHaveConnectedAP = true;
                    GetConnectedAPCache(subNetCache);
                }
                else if (tag == L"BitRate") // BitRate��Ԫ��
                {
                    AddDueElementInfo(bParentChecked, bBitRate_Attr_R, 1);
                    AddDueElementInfo(bParentChecked, bBitRate_Attr_O, 1);
                    if (!m_vn->hasAttr(L"multiplier"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWork��BitRateԪ��ȱ��multiplier����", WtoA(subNetCache.subNetWorkName).c_str());
                        SCDSubNetWorkCheck(msg, bBitRate_Attr_O, etSCDSubNetWorkCheck_BitRate_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"unit"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWork��BitRateԪ��ȱ��unit����", WtoA(subNetCache.subNetWorkName).c_str());
                        SCDSubNetWorkCheck(msg, bBitRate_Attr_R, etSCDSubNetWorkCheck_BitRate_Attr_R);
                    }
                }
            } while (m_vn->toElement(NEXT_SIBLING));
            // ���ظ�Ԫ��
            m_vn->toElement(PARENT);
        }
        // ��Ԫ�ظ澯
        if (!bHaveConnectedAP)
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWorkԪ��ȱ��ConnectedAPԪ��", WtoA(subNetCache.subNetWorkName).c_str());
            SCDSubNetWorkCheck(msg, bChild, etSCDSubNetWorkCheck_Child);
        }        


        SubNetWorkMap::iterator it = m_SCDCache.mapSubNet.find(subNetCache.subNetWorkName);
        if (it != m_SCDCache.mapSubNet.end())
        {
            string msg = "�������Ʋ�Ψһ:SubnetWork name=%s";
            int row = pChecker->CalcRowNumberFromOffset(offset);
            string xml = pChecker->GetRowContentByOffset(offset);
            CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(subNetCache.subNetWorkName).c_str());
        } 
        else
        {
            m_SCDCache.mapSubNet.insert(make_pair(subNetCache.subNetWorkName, subNetCache));
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetConnectedAPCache(SubNetWorkCache &subNetCache)
{
    try
    {
        // �ж������Ƿ�ѡ��
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDConnectedAPCheck_Attr_R").bEnabled;
        ErrorType etSCDConnectedAPCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDConnectedAPCheck_Attr_R").etValue;
        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDConnectedAPCheck_Attr_O").bEnabled;
        ErrorType etSCDConnectedAPCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDConnectedAPCheck_Attr_O").etValue;
        bool bParentChecked = CSCDConnectedAPCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 2); // ConnectedAPԪ�غ���iedName ��apName��2�����ԡ�
        AddDueElementInfo(bParentChecked, bAttr_O, 1); // ConnectedAPԪ�غ���desc��1�����ԡ�

        if (!m_vn->hasAttr(L"iedName"))
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]ConnectedAPԪ��ȱ��iedName����", 
                WtoA(subNetCache.subNetWorkName).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"iedName")).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"apName")).c_str());
            SCDConnectedAPCheck(msg, bAttr_R, etSCDConnectedAPCheck_Attr_R);
        }
        if (!m_vn->hasAttr(L"desc"))
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]ConnectedAPԪ��ȱ��desc����", 
                WtoA(subNetCache.subNetWorkName).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"iedName")).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"apName")).c_str());
            SCDConnectedAPCheck(msg, bAttr_O, etSCDConnectedAPCheck_Attr_O);
        }
        if (!m_vn->hasAttr(L"apName"))
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]ConnectedAPԪ��ȱ��apName����", 
                WtoA(subNetCache.subNetWorkName).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"iedName")).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"apName")).c_str());
            SCDConnectedAPCheck(msg, bAttr_R, etSCDConnectedAPCheck_Attr_R);
        }

        wstring key = VtdGetAttributeValue(m_vn, L"iedName") + L"." + VtdGetAttributeValue(m_vn, L"apName");
        // �ڵ�ǰ�����в���
        ConnectedAPMap::iterator it = subNetCache.mapConnectedAPMap.find(key);
        if (it != subNetCache.mapConnectedAPMap.end())
        {
            ConnectedAPCache *apCache = &it->second;
            GetIPCache(*apCache);
            GetGSECache(*apCache);
            GetSMVCache(*apCache);
            GetPhysConnCache(*apCache);
        }
        else
        {
            ConnectedAPCache apCache;
            apCache.subnetName = subNetCache.subNetWorkName;
            apCache.iedName = VtdGetAttributeValue(m_vn, L"iedName");
            apCache.apName = VtdGetAttributeValue(m_vn, L"apName");
            apCache.key = apCache.iedName + L"." + apCache.apName;

            int offset = GetOffsetOfVTD();
            apCache.offset = offset;

            GetIPCache(apCache);
            GetGSECache(apCache);
            GetSMVCache(apCache);
            GetPhysConnCache(apCache);

            setConnectedAP.insert(apCache.key);
            subNetCache.mapConnectedAPMap.insert(make_pair(apCache.key, apCache));
        }
    }
    catch (...)
    {
    	return false;
    }
    return true;
}

bool CCacheMgr::GetIPCache(ConnectedAPCache &apCache)
{
    try
    {
        // �ж������Ƿ�ѡ��
        bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIPAddressCheck_Child").bEnabled;
        ErrorType etSCDIPAddressCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIPAddressCheck_Child").etValue;

        bool bP_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIPAddressCheck_P_Attr").bEnabled;
        ErrorType etSCDIPAddressCheck_P_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIPAddressCheck_P_Attr").etValue;

        bool bParentChecked = CSCDIPAddressCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bChild, 1); // AddressԪ���Ƿ���һ����ѡ����P
        AddDueElementInfo(bParentChecked, bP_Attr, 2); // P_type����IP, IP_SUBNET,

        if (m_vn->toElement(FIRST_CHILD, L"Address"))
        {
            wstring ip, subNet;

            bool bHaveIP = false;
            bool bHaveSubNet = false;

            if (m_vn->toElement(FIRST_CHILD, L"P"))
            {
                do 
                {
                    wstring type = VtdGetAttributeValue(m_vn, L"type");
                    if (type.compare(L"IP") == 0)
                    {
                        bHaveIP = true;
                        ip = VtdGetTextValue(m_vn);
                    }
                    else if (type.compare(L"IP-SUBNET") == 0)
                    {
                        bHaveSubNet = true;
                        subNet = VtdGetTextValue(m_vn);
                    }

                } while (m_vn->toElement(NEXT_SIBLING, L"P"));
                m_vn->toElement(PARENT);

                if (!bHaveIP)
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]Addressȱ��typeΪIP��P�ڵ�", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                    SCDIPAddressCheck(msg, bP_Attr, etSCDIPAddressCheck_P_Attr);
                }
                if (!bHaveSubNet)
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]Addressȱ��typeΪIP-SUBNET��P�ڵ�", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                    SCDIPAddressCheck(msg, bP_Attr, etSCDIPAddressCheck_P_Attr);
                }
            }
            else
            {
                string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]Address��ȱ��PԪ��", 
                    WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                SCDIPAddressCheck(msg, bChild, etSCDIPAddressCheck_Child);
            }

            if(apCache.ip.IP == L"" && apCache.ip.IPSubNet == L"")
            {
                apCache.ip.IP = ip;
                apCache.ip.IPSubNet = subNet;
            }
            else if(ip != L"" && ip != apCache.ip.IP)
            {
                wstring xml = GetXMLContent();
                int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
                string msg = "IP��ַ���ò�һ��:IED name=%s AP name=%s";
                CIpValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                    WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
            }

            IpValidCheck(apCache.ip);
            IpUniqueCheck(apCache.ip.IP);

            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CCacheMgr::GetGSECache(ConnectedAPCache &apCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"GSE"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_Attr_R").bEnabled;
                ErrorType etSCDGSECheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_Attr_R").etValue;
                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_Attr_O").bEnabled;
                ErrorType etSCDGSECheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_Attr_O").etValue;
                // bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_Child").bEnabled;
                bool bAddress_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_Address_Child").bEnabled;
                ErrorType etSCDGSECheck_Address_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_Address_Child").etValue;
                bool bP_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_P_Attr").bEnabled;
                ErrorType etSCDGSECheck_P_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_P_Attr").etValue;
                bool bMinTime_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_MinTime_Attr").bEnabled;
                ErrorType etSCDGSECheck_MinTime_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_MinTime_Attr").etValue;
                bool bMaxTime_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_MaxTime_Attr").bEnabled;
                ErrorType etSCDGSECheck_MaxTime_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDGSECheck_MaxTime_Attr").etValue;
                bool bParentChecked = CSCDGSECheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 2); // GSEԪ�غ���IdInst��cbName��2������
                AddDueElementInfo(bParentChecked, bAttr_O, 1); // GSEԪ�غ���desc��3������
                // AddDueElementInfo(bParentChecked, bChild, 3); // GSEԪ��Ӧ������Ԫ��Address��MinTime��MaxTime ��Ϊ��ѡ��

                

                ControlBlockAddressCache cbAddrCache;
                cbAddrCache.ldInst = VtdGetAttributeValue(m_vn, L"ldInst");
                cbAddrCache.cbName = VtdGetAttributeValue(m_vn, L"cbName");
                cbAddrCache.key = cbAddrCache.ldInst + L"." + cbAddrCache.cbName;

                if (!m_vn->hasAttr(L"ldInst"))
                {
                    string msg =  FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSEԪ����ȱ��ldInst����", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDGSECheck(msg, bAttr_R, etSCDGSECheck_Attr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSEԪ����ȱ��desc����", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDGSECheck(msg, bAttr_O, etSCDGSECheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"cbName"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSEԪ����ȱ��cbName����", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDGSECheck(msg, bAttr_R, etSCDGSECheck_Attr_R);
                }
               

                int offset = GetOffsetOfVTD();
                cbAddrCache.offset = offset;

                if (m_vn->toElement(FIRST_CHILD, L"Address"))
                {
                    AddDueElementInfo(bParentChecked, bAddress_Child, 1); // AddressԪ���Ƿ���һ����ѡ����P
                    AddDueElementInfo(bParentChecked, bP_Attr, 4); // P_typeӦ����MAC_Address, APPID, VLAN_PRIORITY��VLAN_ID��4������

                    if (m_vn->toElement(FIRST_CHILD, L"P"))
                    {                        
                        bool bHaveMac = false;
                        bool bHaveVLanID = false;
                        bool bHaveVLanPriority = false;
                        bool bHaveAppID = false;

                        do 
                        {
                            wstring type = VtdGetAttributeValue(m_vn, L"type");
                            if (type.compare(L"MAC-Address") == 0)
                            {
                                bHaveMac = true;
                                cbAddrCache.MACAddress = VtdGetTextValue(m_vn);
                            }
                            else if (type.compare(L"VLAN-ID") == 0)
                            {
                                bHaveVLanID = true;
                                cbAddrCache.VLANID = VtdGetTextValue(m_vn);
                            }
                            else if (type.compare(L"APPID") == 0)
                            {
                                bHaveAppID = true;
                                cbAddrCache.APPID = VtdGetTextValue(m_vn);
                            }
                            else if (type.compare(L"VLAN-PRIORITY") == 0)
                            {
                                bHaveVLanPriority = true;
                                cbAddrCache.VLANPRIORITY = VtdGetTextValue(m_vn);
                            }
                        } while (m_vn->toElement(NEXT_SIBLING, L"P"));
                        m_vn->toElement(PARENT);

                        if (!bHaveMac)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Addressȱ��MAC-Address��Ϣ", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDGSECheck(msg, bP_Attr, etSCDGSECheck_P_Attr);
                        }
                        if (!bHaveAppID)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Addressȱ��AppID��Ϣ", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDGSECheck(msg, bP_Attr, etSCDGSECheck_P_Attr);
                        }
                        if (!bHaveVLanID)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Addressȱ��VLanID��Ϣ", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDGSECheck(msg, bP_Attr, etSCDGSECheck_P_Attr);
                        }
                        if (!bHaveVLanPriority)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Addressȱ��VLanPriority��Ϣ", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDGSECheck(msg, bP_Attr, etSCDGSECheck_P_Attr);
                        }
                    }
                    else
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSE��Address��Ԫ��ȱ��P��Ԫ��", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            
                        SCDGSECheck(msg, bAddress_Child, etSCDGSECheck_Address_Child);
                    }
                    m_vn->toElement(PARENT);
                }
                //else
                //{
                //    string msg = "GSE��ȱ��Address��Ԫ��";
                //    SCDGSECheck(msg, bChild);
                //}

                GseAppidValidCheck(cbAddrCache.APPID);
                VlanIdValidCheck(cbAddrCache.VLANID);
                VlanPriorityValidCheck(cbAddrCache.VLANPRIORITY);
                GseMacValidCheck(cbAddrCache.MACAddress);
                MacAddrUniqueCheck(cbAddrCache.MACAddress);
                AppidUniqueCheck(cbAddrCache.APPID);

                if (m_vn->toElement(FIRST_CHILD, L"MinTime"))
                {
                    AddDueElementInfo(bParentChecked, bMinTime_Attr, 2); // MaxTimeԪ�غ���unit��multiplier��2������
                    if (!m_vn->hasAttr(L"unit"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSEԪ����MinTimeԪ��ȱ��unit����", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                        SCDGSECheck(msg, bMinTime_Attr, etSCDGSECheck_MinTime_Attr);
                    }
                    if (!m_vn->hasAttr(L"multiplier"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSEԪ����MinTimeԪ��ȱ��multiplier����", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                        SCDGSECheck(msg, bMinTime_Attr, etSCDGSECheck_MinTime_Attr);
                    }

                    cbAddrCache.MinTime = VtdGetTextValue(m_vn);
                    m_vn->toElement(PARENT);
                }
                //else
                //{
                //    string msg = "GSE��ȱ��MinTime��Ԫ��";
                //    SCDGSECheck(msg, bChild);
                //}

                if (m_vn->toElement(FIRST_CHILD, L"MaxTime"))
                {
                    AddDueElementInfo(bParentChecked, bMaxTime_Attr, 2); // MinTimeԪ�غ���unit��multiplier��2������
                    if (!m_vn->hasAttr(L"unit"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSEԪ����MaxTimeԪ��ȱ��unit����", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                        SCDGSECheck(msg, bMaxTime_Attr, etSCDGSECheck_MaxTime_Attr);
                    }
                    if (!m_vn->hasAttr(L"multiplier"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSEԪ����MaxTimeԪ��ȱ��multiplier����", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                        SCDGSECheck(msg, bMaxTime_Attr, etSCDGSECheck_MaxTime_Attr);
                    }

                    cbAddrCache.MAXTime = VtdGetTextValue(m_vn);
                    m_vn->toElement(PARENT);
                }
                //else
                //{
                //    string msg = "GSE��ȱ��MaxTime��Ԫ��";
                //    SCDGSECheck(msg, bChild);
                //}

                CommunicationGSEorSVUniqueCheck(apCache, cbAddrCache);

                ControlBlockAddressMap::iterator it = apCache.mapGseCBAddress.find(cbAddrCache.key);
                if (it != apCache.mapGseCBAddress.end())
                {
                    // DONothing
                } 
                else
                {
                    apCache.mapGseCBAddress.insert(make_pair(cbAddrCache.key, cbAddrCache));
                }

            } while (m_vn->toElement(NEXT_SIBLING, L"GSE"));
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetPhysConnCache(ConnectedAPCache &apCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"PhysConn"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bAttr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_Attr").bEnabled;
                ErrorType etSCDPhysConnCheck_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_Attr").etValue;

                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_Child").bEnabled;
                ErrorType etSCDPhysConnCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_Child").etValue;

                bool bP_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_P_Attr").bEnabled;
                ErrorType etSCDPhysConnCheck_P_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_P_Attr").etValue;

                bool bParentChecked = CSCDGSECheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr, 1); // PhysConnԪ�غ���type����
                AddDueElementInfo(bParentChecked, bChild, 1); // PhysConnԪ�غ���һ����ѡ����P
                
                
                if (!m_vn->hasAttr(L"type"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s]PhysConnԪ��ȱ��type����", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                    SCDPhysConnCheck(msg, bAttr, etSCDPhysConnCheck_Attr);
                }

                PhysConnCache pcCache;
                pcCache.PhysConnType = VtdGetAttributeValue(m_vn, L"type");
                pcCache.offset = GetOffsetOfVTD();

                if (m_vn->toElement(FIRST_CHILD, L"P"))
                {
                    AddDueElementInfo(bParentChecked, bP_Attr, 2); // P_typeӦ����Type��Plug ��2������

                    bool bHavePlug = false;
                    bool bHaveType = false;
                    do 
                    {
                        wstring type = VtdGetAttributeValue(m_vn, L"type");
                        if (type.compare(L"Port") == 0)
                        {
                            pcCache.Port = VtdGetTextValue(m_vn);
                        }
                        else if (type.compare(L"Plug") == 0)
                        {
                            bHavePlug = true;
                            pcCache.Plug = VtdGetTextValue(m_vn);
                        }
                        else if (type.compare(L"Type") == 0)
                        {
                            bHaveType = true;
                            pcCache.Type = VtdGetTextValue(m_vn);
                        }
                    } while (m_vn->toElement(NEXT_SIBLING, L"P"));
                    
                    if (!bHaveType)
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s]PhysConnȱ��typeΪType��PԪ��", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                        SCDPhysConnCheck(msg, bP_Attr, etSCDPhysConnCheck_P_Attr);
                    }
                    if (!bHavePlug)
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s]PhysConnȱ��typeΪPlug��PԪ��", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                        SCDPhysConnCheck(msg, bP_Attr, etSCDPhysConnCheck_P_Attr);
                    }

                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s]PhysConnԪ��ȱ��P��Ԫ��", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                    SCDPhysConnCheck(msg, bChild, etSCDPhysConnCheck_Child);
                }

                PhysConnMap::iterator it = apCache.mapPhysConn.find(pcCache.Port);
                if (it != apCache.mapPhysConn.end())
                {
                    string msg = "ConnectedAP��PhysConn�����ظ�:SubnetWork name=%s ConnectedAP iedName=%s apName=%s PhysConn type=%s Port=%s";
                    int row = pChecker->CalcRowNumberFromOffset(pcCache.offset);
                    string xml = pChecker->GetRowContentByOffset(pcCache.offset);
                    CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(pcCache.PhysConnType).c_str(), WtoA(pcCache.Port).c_str());
                }
                else
                {
                    apCache.mapPhysConn.insert(make_pair(pcCache.Port, pcCache));
                }
            } while (m_vn->toElement(NEXT_SIBLING, L"PhysConn"));
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetSMVCache(ConnectedAPCache &apCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"SMV"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSMVCheck_Attr_R").bEnabled;
                ErrorType etSCDSMVCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSMVCheck_Attr_R").etValue;

                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSMVCheck_Attr_O").bEnabled;
                ErrorType etSCDSMVCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSMVCheck_Attr_O").etValue;
                
                // bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSMVCheck_Child").bEnabled;
                bool bAddress_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSMVCheck_Address_Child").bEnabled;
                ErrorType etSCDSMVCheck_Address_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSMVCheck_Address_Child").etValue;

                bool bP_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSMVCheck_P_Attr").bEnabled;
                ErrorType etSCDSMVCheck_P_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSMVCheck_P_Attr").etValue;

                //bool bMaxTime_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSMVCheck_MinTime_Attr").bEnabled;
                //bool bMinTime_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDSMVCheck_MaxTime_Attr").bEnabled;
                bool bParentChecked = CSCDSMVCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 2); // SMVԪ�غ���desc, IdInst��cbName��3������
                AddDueElementInfo(bParentChecked, bAttr_O, 1); // SMVԪ�غ���desc, IdInst��cbName��3������
                // AddDueElementInfo(bParentChecked, bChild, 3); // SMVԪ��Ӧ������Ԫ��Address��MinTime��MaxTime ��Ϊ��ѡ�� 
                //AddDueElementInfo(bParentChecked, bMaxTime_Attr, 2); // MinTimeԪ�غ���unit��multiplier��2������
                //AddDueElementInfo(bParentChecked, bMinTime_Attr, 2); // MaxTimeԪ�غ���unit��multiplier��2������

                

                ControlBlockAddressCache cbAddrCache;
                cbAddrCache.ldInst = VtdGetAttributeValue(m_vn, L"ldInst");
                cbAddrCache.cbName = VtdGetAttributeValue(m_vn, L"cbName");
                cbAddrCache.key = cbAddrCache.ldInst + L"." + cbAddrCache.cbName;

                if (!m_vn->hasAttr(L"ldInst"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,SMV ldInst=%s cbName=%s]SMVԪ����ȱ��ldInst����", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDSMVCheck(msg, bAttr_R, etSCDSMVCheck_Attr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,SMV ldInst=%s cbName=%s]SMVԪ����ȱ��desc����", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDSMVCheck(msg, bAttr_O, etSCDSMVCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"cbName"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,SMV ldInst=%s cbName=%s]SMVԪ����ȱ��cbName����", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDSMVCheck(msg, bAttr_R, etSCDSMVCheck_Attr_R);
                }

                int offset = GetOffsetOfVTD();
                cbAddrCache.offset = offset;

                if (m_vn->toElement(FIRST_CHILD, L"Address"))
                {
                    AddDueElementInfo(bParentChecked, bAddress_Child, 1); // AddressԪ���Ƿ���һ����ѡ����P
                    AddDueElementInfo(bParentChecked, bP_Attr, 4); // P_typeӦ����MAC_Address, APPID, VLAN_PRIORITY��VLAN_ID��4������

                    if (m_vn->toElement(FIRST_CHILD, L"P"))
                    {
                        bool bHaveMac = false;
                        bool bHaveVLanID = false;
                        bool bHaveVLanPriority = false;
                        bool bHaveAppID = false;

                        do 
                        {
                            wstring type = VtdGetAttributeValue(m_vn, L"type");
                            if (type.compare(L"MAC-Address") == 0)
                            {
                                bHaveMac = true;
                                cbAddrCache.MACAddress = VtdGetTextValue(m_vn);
                            }
                            else if (type.compare(L"VLAN-ID") == 0)
                            {
                                bHaveVLanID = true;
                                cbAddrCache.VLANID = VtdGetTextValue(m_vn);
                            }
                            else if (type.compare(L"APPID") == 0)
                            {
                                bHaveAppID = true;
                                cbAddrCache.APPID = VtdGetTextValue(m_vn);
                            }
                            else if (type.compare(L"VLAN-PRIORITY") == 0)
                            {
                                bHaveVLanPriority = true;
                                cbAddrCache.VLANPRIORITY = VtdGetTextValue(m_vn);
                            }
                        } while (m_vn->toElement(NEXT_SIBLING, L"P"));
                        m_vn->toElement(PARENT);

                        if (!bHaveMac)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Addressȱ��MAC-Address��Ϣ", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDSMVCheck(msg, bP_Attr, etSCDSMVCheck_P_Attr);
                        }
                        if (!bHaveAppID)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Addressȱ��APPID��Ϣ", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDSMVCheck(msg, bP_Attr, etSCDSMVCheck_P_Attr);
                        }
                        if (!bHaveVLanID)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Addressȱ��VLanID��Ϣ", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDSMVCheck(msg, bP_Attr, etSCDSMVCheck_P_Attr);
                        }
                        if (!bHaveVLanPriority)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Addressȱ��VLanPriority��Ϣ", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDSMVCheck(msg, bP_Attr, etSCDSMVCheck_P_Attr);
                        }                        
                    }
                    //else
                    //{
                    //    string msg = "SMV��Address��Ԫ��ȱ��P��Ԫ��";
                    //    SCDSMVCheck(msg, bChild);
                    //}
                    
                    m_vn->toElement(PARENT);
                }
                //else
                //{
                //    string msg = "SMV��ȱ��Address��Ԫ��";
                //    SCDSMVCheck(msg, bChild);
                //}

                SmvAppidValidCheck(cbAddrCache.APPID);
                VlanIdValidCheck(cbAddrCache.VLANID);
                VlanPriorityValidCheck(cbAddrCache.VLANPRIORITY);
                SmvMacValidCheck(cbAddrCache.MACAddress);
                AppidUniqueCheck(cbAddrCache.APPID);
                MacAddrUniqueCheck(cbAddrCache.MACAddress);
                CommunicationGSEorSVUniqueCheck(apCache, cbAddrCache);

                ControlBlockAddressMap::iterator it = apCache.mapSmvCBAddress.find(cbAddrCache.key);
                if (it != apCache.mapSmvCBAddress.end())
                {
                    // DONothing
                } 
                else
                {
                    apCache.mapSmvCBAddress.insert(make_pair(cbAddrCache.key, cbAddrCache));
                }
            } while (m_vn->toElement(NEXT_SIBLING, L"SMV"));
            m_vn->toElement(PARENT);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

// DataTypeTemplates Cache
bool CCacheMgr::GetDataTypeTemplateCache()
{
    // ʵ��������DOI/SDI/DAI�Ķ��������Ƿ�����������ģ�嶨������ݶ���ṹһ��
    // ����δ�����õ�LNType��DOtype��DAtype
    if (!IsNeededCacheMarkExists("DataTypeTemplates"))
    {
        return true;
    }
    try
    {
        m_vn->toElement(ROOT);
        if (m_vn->toElement(FIRST_CHILD, L"DataTypeTemplates"))
        {
            GetLNodeTypeCache(); 
            GetDOTypeCache();
            GetDATypeCache();
            GetEnumTypeCache(); // Add
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CCacheMgr::GetLNodeTypeCache()
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"LNodeType"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDLNodeTypeCheck_Attr_R").bEnabled;
                ErrorType etSCDLNodeTypeCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDLNodeTypeCheck_Attr_R").etValue;

                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDLNodeTypeCheck_Attr_O").bEnabled;
                ErrorType etSCDLNodeTypeCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDLNodeTypeCheck_Attr_O").etValue;

                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDLNodeTypeCheck_Child").bEnabled;
                ErrorType etSCDLNodeTypeCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDLNodeTypeCheck_Child").etValue;

                bool bDO_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDLNodeTypeCheck_DO_Attr_O").bEnabled;
                ErrorType etSCDLNodeTypeCheck_DO_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDLNodeTypeCheck_DO_Attr_O").etValue;

                bool bDO_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDLNodeTypeCheck_DO_Attr_R").bEnabled;
                ErrorType etSCDLNodeTypeCheck_DO_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDLNodeTypeCheck_DO_Attr_R").etValue;

                bool bParentChecked = CSCDLNodeTypeCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 2); // LNodeTypeԪ�غ���id,InClass��2�����ԡ�
                AddDueElementInfo(bParentChecked, bAttr_O, 2); // LNodeTypeԪ�غ���desc, ideType��2�����ԡ�
                AddDueElementInfo(bParentChecked, bChild, 1); // LNodeTypeԪ�غ�����Ԫ��DO���ݶ���Ԫ�ء�
                
                string id = WtoA(VtdGetAttributeValue(m_vn, L"id"));
                if (!m_vn->hasAttr(L"id"))
                {
                    string msg = FormatXMLMessage("[LNodeType id=%s]LNodeTypeԪ��ȱ��id����", id.c_str());
                    SCDLNodeTypeCheck(msg, bAttr_R, etSCDLNodeTypeCheck_Attr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[LNodeType id=%s]LNodeTypeԪ��ȱ��desc����", id.c_str());
                    SCDLNodeTypeCheck(msg, bAttr_O, etSCDLNodeTypeCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"iedType"))
                {
                    string msg = FormatXMLMessage("[LNodeType id=%s]LNodeTypeԪ��ȱ��iedType����", id.c_str());
                    SCDLNodeTypeCheck(msg, bAttr_O, etSCDLNodeTypeCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"lnClass"))
                {
                    string msg = FormatXMLMessage("[LNodeType id=%s]LNodeTypeԪ��ȱ��lnClass����", id.c_str());
                    SCDLNodeTypeCheck(msg, bAttr_R, etSCDLNodeTypeCheck_Attr_R);
                }

                LNodeTypeCache lNodeType;
                lNodeType.offset = GetOffsetOfVTD();
                lNodeType.id = VtdGetAttributeValue(m_vn, L"id");
                lNodeType.lnClass = VtdGetAttributeValue(m_vn, L"lnClass");
                lNodeType.bUnUsed = true;

                // Get LNodeType Do
                if (m_vn->toElement(FIRST_CHILD, L"DO"))
                {
                    wstring valueDoName, valueDoType;
                    do 
                    {
                        AddDueElementInfo(bParentChecked, bDO_Attr_R, 2); // DOԪ�غ���name, type 2�����ԡ�
                        AddDueElementInfo(bParentChecked, bDO_Attr_O, 2); // DOԪ�غ���accessControl��transient��2�����ԡ�

                        DOCache doName;
                        doName.offset = GetOffsetOfVTD();
                        doName.name = VtdGetAttributeValue(m_vn, L"name");
                        doName.type = VtdGetAttributeValue(m_vn, L"type");
                        doName.bUnUsed = true;

                        if (!m_vn->hasAttr(L"name"))
                        {
                            string msg = FormatXMLMessage("[LNodeType id=%s DO name=%s]DOȱ��name����", id.c_str(), WtoA(doName.name).c_str());
                            SCDLNodeTypeCheck(msg, bDO_Attr_R, etSCDLNodeTypeCheck_DO_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"type"))
                        {
                            string msg = FormatXMLMessage("[LNodeType id=%s DO name=%s]DOȱ��type����", id.c_str(), WtoA(doName.name).c_str());
                            SCDLNodeTypeCheck(msg, bDO_Attr_R, etSCDLNodeTypeCheck_DO_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"accessControl"))
                        {
                            string msg = FormatXMLMessage("[LNodeType id=%s DO name=%s]DOȱ��accessControl����", id.c_str(), WtoA(doName.name).c_str());
                            SCDLNodeTypeCheck(msg, bDO_Attr_O, etSCDLNodeTypeCheck_DO_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"transient"))
                        {
                            string msg = FormatXMLMessage("[LNodeType id=%s DO name=%s]DOȱ��transient����", id.c_str(), WtoA(doName.name).c_str());
                            SCDLNodeTypeCheck(msg, bDO_Attr_O, etSCDLNodeTypeCheck_DO_Attr_O);
                        }

                        lNodeType.mapDO.insert(make_pair(doName.name, doName));
                    } while (m_vn->toElement(NEXT_SIBLING, L"DO"));
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[LNodeType id=%s]LNodeTypeԪ��ȱ��DO��Ԫ��", id.c_str());
                    SCDLNodeTypeCheck(msg, bChild, etSCDLNodeTypeCheck_Child);
                }

                m_SCDCache.sDTTCache.mapLNode.insert(make_pair(lNodeType.id, lNodeType));
            } while (m_vn->toElement(NEXT_SIBLING, L"LNodeType"));

            m_vn->toElement(PARENT);
        }

    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetDOTypeCache()
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"DOType"))
        {
            do 
            {
                // �ж������Ƿ�ѡ��
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_Attr_R").bEnabled;
                ErrorType etSCDDOTypeCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_Attr_R").etValue;
                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_Attr_O").bEnabled;
                ErrorType etSCDDOTypeCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_Attr_O").etValue;
                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_Child_O").bEnabled;
                ErrorType etSCDDOTypeCheck_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_Child_O").etValue;
                bool bSDO_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_SDO_Attr_R").bEnabled;
                ErrorType etSCDDOTypeCheck_SDO_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_SDO_Attr_R").etValue;
                bool bSDO_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_SDO_Attr_O").bEnabled;
                ErrorType etSCDDOTypeCheck_SDO_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_SDO_Attr_O").etValue;
                bool bDA_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_DA_Attr_R").bEnabled;
                ErrorType etSCDDOTypeCheck_DA_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_DA_Attr_R").etValue;
                bool bDA_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_DA_Attr_O").bEnabled;
                ErrorType etSCDDOTypeCheck_DA_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDOTypeCheck_DA_Attr_O").etValue;
                bool bParentChecked = CSCDDOTypeCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 2); // DOTypeԪ�غ���id, cdc��2������
                AddDueElementInfo(bParentChecked, bAttr_O, 1); // DOTypeԪ�غ���iedType����1������
                AddDueElementInfo(bParentChecked, bChild, 2); // DOTypeԪ�غ�����Ԫ�أ�SDO�����ݶ���Ԫ�ء�DA��������Ԫ�ء���Ϊ��ѡԪ��
                              

                DOTypeCache doTypeCache;
                doTypeCache.offset = GetOffsetOfVTD();
                doTypeCache.id = VtdGetAttributeValue(m_vn, L"id");
                doTypeCache.cdc = VtdGetAttributeValue(m_vn, L"cdc");
                doTypeCache.bUnUsed = true;

                if (!m_vn->hasAttr(L"id"))
                {
                    string msg = FormatXMLMessage("[DOType id=%s]DOTypeԪ��ȱ��id����", WtoA(doTypeCache.id).c_str());
                    SCDDOTypeCheck(msg, bAttr_R, etSCDDOTypeCheck_Attr_R);
                }
                if (!m_vn->hasAttr(L"iedType"))
                {
                    string msg = FormatXMLMessage("[DOType id=%s]DOTypeԪ��ȱ��iedType����", WtoA(doTypeCache.id).c_str());
                    SCDDOTypeCheck(msg, bAttr_O, etSCDDOTypeCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"cdc"))
                {
                    string msg = FormatXMLMessage("[DOType id=%s]DOTypeԪ��ȱ��cdc����", WtoA(doTypeCache.id).c_str());
                    SCDDOTypeCheck(msg, bAttr_R, etSCDDOTypeCheck_Attr_R);
                }

                // Get LNodeType Do
                if (m_vn->toElement(FIRST_CHILD, L"DA"))
                {
                    do 
                    {
                        AddDueElementInfo(bParentChecked, bDA_Attr_R, 3); // DAԪ�غ���name, fc, bType 3������
                        AddDueElementInfo(bParentChecked, bDA_Attr_O, 1); // DAԪ�غ���type 1������
                        

                        DACache daTypeCache;
                        daTypeCache.offset = GetOffsetOfVTD();
                        daTypeCache.name = VtdGetAttributeValue(m_vn, L"name");
                        daTypeCache.bType = VtdGetAttributeValue(m_vn, L"bType");
                        daTypeCache.type = VtdGetAttributeValue(m_vn, L"type");
                        daTypeCache.bUnUsed = true;

                        if (!m_vn->hasAttr(L"name"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s DA name=%s]DAԪ��ȱ��name����", WtoA(doTypeCache.id).c_str(), WtoA(daTypeCache.name).c_str());
                            SCDDOTypeCheck(msg, bDA_Attr_R, etSCDDOTypeCheck_DA_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"fc"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s DA name=%s]DAԪ��ȱ��fc����", WtoA(doTypeCache.id).c_str(), WtoA(daTypeCache.name).c_str());
                            SCDDOTypeCheck(msg, bDA_Attr_R, etSCDDOTypeCheck_DA_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"bType"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s DA name=%s]DAԪ��ȱ��bType����", WtoA(doTypeCache.id).c_str(), WtoA(daTypeCache.name).c_str());
                            SCDDOTypeCheck(msg, bDA_Attr_R, etSCDDOTypeCheck_DA_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"type"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s DA name=%s]DAԪ��ȱ��type����", WtoA(doTypeCache.id).c_str(), WtoA(daTypeCache.name).c_str());
                            SCDDOTypeCheck(msg, bDA_Attr_O, etSCDDOTypeCheck_DA_Attr_O);
                        }

                        doTypeCache.mapDaType.insert(make_pair(daTypeCache.name, daTypeCache));
                    } while (m_vn->toElement(NEXT_SIBLING, L"DA"));
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[DOType id=%s]DOTypeԪ��ȱ��DA��Ԫ��", WtoA(doTypeCache.id).c_str());
                    SCDDOTypeCheck(msg, bChild, etSCDDOTypeCheck_Child_O);
                }
                // Get SDO Info
                if (m_vn->toElement(FIRST_CHILD, L"SDO"))
                {
                    do 
                    {
                        AddDueElementInfo(bParentChecked, bSDO_Attr_R, 2); // SDOԪ�غ���name, type��2������
                        AddDueElementInfo(bParentChecked, bSDO_Attr_O, 1); // SDOԪ�غ���desc��1������
                        

                        DOCache sDoCache;
                        sDoCache.offset = GetOffsetOfVTD();
                        sDoCache.name = VtdGetAttributeValue(m_vn, L"name");
                        sDoCache.type = VtdGetAttributeValue(m_vn, L"type");
                        sDoCache.bUnUsed = true;

                        if (!m_vn->hasAttr(L"name"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s SDO name=%s]SDOԪ��ȱ��name����", WtoA(doTypeCache.id).c_str(), WtoA(sDoCache.name).c_str());
                            SCDDOTypeCheck(msg, bSDO_Attr_R, etSCDDOTypeCheck_SDO_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"desc"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s SDO name=%s]SDOԪ��ȱ��desc����", WtoA(doTypeCache.id).c_str(), WtoA(sDoCache.name).c_str());
                            SCDDOTypeCheck(msg, bSDO_Attr_O, etSCDDOTypeCheck_SDO_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"type"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s SDO name=%s]SDOԪ��ȱ��type����", WtoA(doTypeCache.id).c_str(), WtoA(sDoCache.name).c_str());
                            SCDDOTypeCheck(msg, bSDO_Attr_R, etSCDDOTypeCheck_SDO_Attr_R);
                        }

                        doTypeCache.mapSDO.insert(make_pair(sDoCache.name, sDoCache));
                    } while (m_vn->toElement(NEXT_SIBLING, L"SDO"));

                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[DOType id=%s]DOTypeԪ��ȱ��SDO��Ԫ��", WtoA(doTypeCache.id).c_str());
                    SCDDOTypeCheck(msg, bChild, etSCDDOTypeCheck_Child_O);
                }

                m_SCDCache.sDTTCache.mapDoType.insert(make_pair(doTypeCache.id, doTypeCache));
            } while (m_vn->toElement(NEXT_SIBLING, L"DOType"));

            m_vn->toElement(PARENT);
        }

    }
    catch (...)
    {
    	return false;
    }
    return true;
}

bool CCacheMgr::GetDATypeCache()
{
    if (m_vn->toElement(FIRST_CHILD, L"DAType"))
    {
        do 
        {
            // �ж������Ƿ�ѡ��
            bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_Attr_R").bEnabled;
            ErrorType etSCDDATypeCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_Attr_R").etValue;

            bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_Attr_O").bEnabled;
            ErrorType etSCDDATypeCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_Attr_O").etValue;

            bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_Child").bEnabled;
            ErrorType etSCDDATypeCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_Child").etValue;

            bool bBDA_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_BDA_Attr_R").bEnabled;
            ErrorType etSCDDATypeCheck_BDA_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_BDA_Attr_R").etValue;

            bool bBDA_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_BDA_Attr_O").bEnabled;
            ErrorType etSCDDATypeCheck_BDA_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_BDA_Attr_O").etValue;
            //bool bBDA_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_BDA_Child").bEnabled;
            //bool bVal_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDDATypeCheck_Val_Attr").bEnabled;
            bool bParentChecked = CSCDDATypeCheck::GetInstance()->IsEnabled();
            AddDueElementInfo(bParentChecked, bAttr_R, 1); // DATypeԪ�غ���id��1������
            AddDueElementInfo(bParentChecked, bAttr_O, 2); // DATypeԪ�غ���iedType��desc��2������
            AddDueElementInfo(bParentChecked, bChild, 1); // DATypeԪ�غ��б�ѡ��Ԫ�أ�BDA������������Ԫ��
                      
            DATypeCache daTypeCache;
            daTypeCache.offset = GetOffsetOfVTD();
            daTypeCache.id = VtdGetAttributeValue(m_vn, L"id");
            daTypeCache.bUnUsed = true;

            if (!m_vn->hasAttr(L"id"))
            {
                string msg = FormatXMLMessage("[DAType id=%s]DATypeԪ��ȱ��id����", WtoA(daTypeCache.id).c_str());
                SCDDATypeCheck(msg, bAttr_R, etSCDDATypeCheck_Attr_R);
            }
            if (!m_vn->hasAttr(L"iedType"))
            {
                string msg = FormatXMLMessage("[DAType id=%s]DATypeԪ��ȱ��iedType����", WtoA(daTypeCache.id).c_str());
                SCDDATypeCheck(msg, bAttr_O, etSCDDATypeCheck_Attr_O);
            }
            if (!m_vn->hasAttr(L"desc"))
            {
                string msg = FormatXMLMessage("[DAType id=%s]DATypeԪ��ȱ��desc����", WtoA(daTypeCache.id).c_str());
                SCDDATypeCheck(msg, bAttr_O, etSCDDATypeCheck_Attr_O);
            }

            // Get LNodeType Do
            if (m_vn->toElement(FIRST_CHILD, L"BDA"))
            {
                do 
                {
                    AddDueElementInfo(bParentChecked, bBDA_Attr_R, 2); // BDAԪ�غ���name, bType��2������
                    AddDueElementInfo(bParentChecked, bBDA_Attr_O, 1); // BDAԪ�غ���type��1������
                    

                    BDACache bdaCache;
                    bdaCache.offset = GetOffsetOfVTD();
                    bdaCache.name = VtdGetAttributeValue(m_vn, L"name");
                    bdaCache.bType = VtdGetAttributeValue(m_vn, L"bType");
                    bdaCache.type = VtdGetAttributeValue(m_vn, L"type");
                    bdaCache.bUnUsed = true;

                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[DAType id=%s BDA name=%s]BDAԪ��ȱ��name����", WtoA(daTypeCache.id).c_str(), WtoA(bdaCache.name).c_str());
                        SCDDATypeCheck(msg, bBDA_Attr_R, etSCDDATypeCheck_BDA_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"bType"))
                    {
                        string msg = FormatXMLMessage("[DAType id=%s BDA name=%s]BDAԪ��ȱ��bType����", WtoA(daTypeCache.id).c_str(), WtoA(bdaCache.name).c_str());
                        SCDDATypeCheck(msg, bBDA_Attr_R, etSCDDATypeCheck_BDA_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"type"))
                    {
                        string msg = FormatXMLMessage("[DAType id=%s BDA name=%s]BDAԪ��ȱ��type����", WtoA(daTypeCache.id).c_str(), WtoA(bdaCache.name).c_str());
                        SCDDATypeCheck(msg, bBDA_Attr_O, etSCDDATypeCheck_BDA_Attr_O);
                    }

                    daTypeCache.mapBDA.insert(make_pair(bdaCache.name, bdaCache));

                } while (m_vn->toElement(NEXT_SIBLING, L"BDA"));

                m_vn->toElement(PARENT);
            }
            else
            {
                string msg = FormatXMLMessage("[DAType id=%s]DATypeԪ��ȱ��BDA��Ԫ��", WtoA(daTypeCache.id).c_str());
                SCDDATypeCheck(msg, bChild, etSCDDATypeCheck_Child);
            }

            m_SCDCache.sDTTCache.mapDaType.insert(make_pair(daTypeCache.id, daTypeCache));
        } while (m_vn->toElement(NEXT_SIBLING, L"DAType"));

        m_vn->toElement(PARENT);
    }
    return true;
}

bool CCacheMgr::GetEnumTypeCache()
{
    if (m_vn->toElement(FIRST_CHILD, L"EnumType"))
    {
        do 
        {
            // �ж������Ƿ�ѡ��
            bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Attr_R").bEnabled;
            ErrorType etSCDEnumTypeCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Attr_R").etValue;

            bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Attr_O").bEnabled;
            ErrorType etSCDEnumTypeCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Attr_O").etValue;

            bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Child").bEnabled;
            ErrorType etSCDEnumTypeCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Child").etValue;

            bool bEnumVal_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_EnumVal_Attr").bEnabled;
            ErrorType etSCDEnumTypeCheck_EnumVal_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_EnumVal_Attr").etValue;

            bool bParentChecked = CSCDEnumTypeCheck::GetInstance()->IsEnabled();
            AddDueElementInfo(bParentChecked, bAttr_R, 1); // EnumTypeԪ�غ���id��1������
            AddDueElementInfo(bParentChecked, bAttr_O, 1); // EnumTypeԪ�غ���desc��1������
            AddDueElementInfo(bParentChecked, bChild, 1); // EnumTypeԪ�غ��б�ѡ��Ԫ�أ�EnumValö��ֵԪ��
                      


            EnumTypeCache enumTypeCache;
            enumTypeCache.offset = GetOffsetOfVTD();
            enumTypeCache.id = VtdGetAttributeValue(m_vn, L"id");
            enumTypeCache.bUnUsed = true;

            if (!m_vn->hasAttr(L"id"))
            {
                string msg = FormatXMLMessage("[EnumType id=%s]EnumTypeԪ��ȱ��id����", WtoA(enumTypeCache.id).c_str());
                SCDEnumTypeCheck(msg, bAttr_R, etSCDEnumTypeCheck_Attr_R);
            }
            if (!m_vn->hasAttr(L"desc"))
            {
                string msg = FormatXMLMessage("[EnumType id=%s]EnumTypeԪ��ȱ��desc����", WtoA(enumTypeCache.id).c_str());
                SCDEnumTypeCheck(msg, bAttr_O, etSCDEnumTypeCheck_Attr_O);
            }

            // Get LNodeType Do
            if (m_vn->toElement(FIRST_CHILD, L"EnumVal"))
            {
                do 
                {
                    AddDueElementInfo(bParentChecked, bEnumVal_Attr, 1); // EnumValԪ�غ���ord����
                    if (!m_vn->hasAttr(L"ord"))
                    {
                        string msg = FormatXMLMessage("[EnumType id=%s]EnumValԪ��ȱ��ord����", WtoA(enumTypeCache.id).c_str());
                        SCDEnumTypeCheck(msg, bEnumVal_Attr, etSCDEnumTypeCheck_EnumVal_Attr);
                    }

                    wstring ord = VtdGetAttributeValue(m_vn, L"ord");
                    wstring val = VtdGetTextValue(m_vn);

                    enumTypeCache.mapEnumVal.insert(make_pair(ord, val));
                } while (m_vn->toElement(NEXT_SIBLING, L"EnumVal"));
                m_vn->toElement(PARENT);
            }
            else
            {
                string msg = FormatXMLMessage("[EnumType id=%s]EnumTypeԪ��ȱ��EnumVal��Ԫ��", WtoA(enumTypeCache.id).c_str());
                SCDEnumTypeCheck(msg, bChild, etSCDEnumTypeCheck_Child);
            }


            m_SCDCache.sDTTCache.mapEnumType.insert(make_pair(enumTypeCache.id, enumTypeCache));
        } while (m_vn->toElement(NEXT_SIBLING, L"EnumType"));

        m_vn->toElement(PARENT);
    }
    return true;
}


//-------------------------------------------------------�����ڼ����--------------------------------------------------------------


// Communication��SMV��APPID��ֵ�Ƿ�Ϊ4λ16����ֵ���Ƿ���4000��7FFFȡֵ��Χ���Ҳ���Ϊ0
bool CCacheMgr::SmvAppidValidCheck(wstring &appid)
{
    if (CSmvAppidValidCheck::GetInstance()->IsEnabled())
    {
        if (appid < L"4000" || appid > L"7FFF" || appid.size() != 4)
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "Communication��SMV��APPID��ֵ����4λ16����ֵ������4000��7FFFȡֵ��Χ��: APPID value=%s";
            CSmvAppidValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(appid).c_str());
        }
    }
    return true;
}

// Communication��GSE��APPID��ֵ�Ƿ�Ϊ4λ16����ֵ���Ƿ���0000��3FFFȡֵ��Χ��
bool CCacheMgr::GseAppidValidCheck(wstring &appid)
{
    if (CGseAppidValidCheck::GetInstance()->IsEnabled())
    {
        if (appid < L"0000" || appid > L"3FFF" || appid.size()!=4)
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "Communication��GSE��APPID��ֵ����4λ16����ֵ������0000��3FFFȡֵ��Χ��:APPID value=%s";
            CGseAppidValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(appid).c_str());
        }
    }
    return true;
}

// Communication��SMV��GSE��VLAN-ID�Ƿ�Ϊ3λ16����ֵ
bool CCacheMgr::VlanIdValidCheck(wstring &vlanID)
{
    if (CVlanValidCheck::GetInstance()->IsEnabled())
    {
        if (vlanID < L"000" || vlanID > L"FFF" || vlanID.size() != 3)
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "Communication��SMV��GSE��VLAN-ID����3λ16����ֵ:VLan-ID value=%s";
            CVlanValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(vlanID).c_str());
        }
    }
    return true;
}

bool CCacheMgr::VlanPriorityValidCheck(wstring &vlanPoy)
{
    if (CVlanValidCheck::GetInstance()->IsEnabled())
    {
        if (vlanPoy < L"0" || vlanPoy > L"7" || vlanPoy.size() != 1)
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "Communication��SMV��GSE��VLAN-PRIORITY����0~7��:VLAN-PRIORITY value=%s";
            CVlanValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(vlanPoy).c_str());
        }
    }
    return true;
}

// GSE��MAC��ַ��01-0C-CD-01-00-00~01-0C-CD-01-FF-FF��Χ��
bool CCacheMgr::GseMacValidCheck(wstring &mac)
{
    if (CGseMacValidCheck::GetInstance()->IsEnabled())
    {
        if (mac.size() == 0 || mac < L"01-0C-CD-01-00-00" || mac > L"01-0C-CD-01-FF-FF")
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "GSE��MAC��ַ����01-0C-CD-01-00-00~01-0C-CD-01-FF-FF��Χ��: MAC-Address value=%s";
            CGseMacValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(mac).c_str());
        }
    }
    return true;
}

// SMV��MAC��ַ��01-0C-CD-04-00-00~01-0C-CD-04-FF-FF��Χ��
bool CCacheMgr::SmvMacValidCheck(wstring &mac)
{
    if (CSmvMacValidCheck::GetInstance()->IsEnabled())
    {
        if (mac < L"01-0C-CD-04-00-00" || mac > L"01-0C-CD-04-FF-FF")
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "SMV��MAC��ַ����01-0C-CD-04-00-00~01-0C-CD-04-FF-FF��Χ��: MAC-Address value=%s";
            CSmvMacValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(mac).c_str());
        }
    }
    return true;
}

// IP��ַ�����������ÿһλ��0~255��Χ��
bool CCacheMgr::IpValidCheck(IPCache &ip)
{
    if (CIpValidCheck::GetInstance()->IsEnabled())
    {
        if (ip.IP.size() == 0 || !CIpValidCheck::GetInstance()->is_ipaddr(WtoA(ip.IP).c_str()))
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "IP��ַȡֵ����0~255��Χ��:IP value=%s";
            CIpValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(ip.IP).c_str());
        }
        if (ip.IPSubNet.size() == 0 || !CIpValidCheck::GetInstance()->is_ipaddr(WtoA(ip.IPSubNet).c_str()))
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "���������ȡֵ��Χ����0~255��: IP-SUBNET value=%s";
            CIpValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(ip.IPSubNet).c_str());
        }
    }
    return true;
}

// Communication��Goose SVͨ�������ظ����
bool CCacheMgr::CommunicationGSEorSVUniqueCheck(ConnectedAPCache &apCache, ControlBlockAddressCache &cbAddrCache)
{
    if (CCommunicationGSEorSVUniqueCheck::GetInstance()->IsEnabled())
    {
        wstring key = apCache.key + L"." + cbAddrCache.key;
        if (setGooseOrSV.count(key) == 1)
        {
            string msg = "������GOOSE��SV�����ظ�:SubnetWork name=%s ConnectedAP iedName=%s apName=%s ldInst=%s cbName=%s";
            int row = pChecker->CalcRowNumberFromOffset(cbAddrCache.offset);
            string xml = pChecker->GetRowContentByRowNumber(row);
            CCommunicationGSEorSVUniqueCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
        }
        else
        {
            setGooseOrSV.insert(key);
        }
    }
    return true;
}

// Communication��SMV��GSE�е�APPID�Ƿ�ȫ��Ψһ
bool CCacheMgr::AppidUniqueCheck(wstring &appid)
{
    if (CAppidUniqueCheck::GetInstance()->IsEnabled())
    {
        if (appid.size() == 0)
        {
            return true;
        }
        if (setAppID.count(appid) == 1)
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "APPID��Ψһ:APPID value=%s";
            CAppidUniqueCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(appid).c_str());
        }
        else
        {
            setAppID.insert(appid);
        }
    }
    return true;
}

// SCD�ļ���Communication��MAC-Address��ֵ�Ƿ�ȫ��Ψһ
bool CCacheMgr::MacAddrUniqueCheck(wstring &mac)
{
    if (CMacAddrUniqueCheck::GetInstance()->IsEnabled())
    {
        // ��MacΪ�գ���ͨ�Ų�����ȷ�Դ���
        if (mac.size() == 0)
        {
            return true;
        }
        if (setMac.count(mac) == 1)
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "Mac��ַ�����ظ�:MAC-Address value=%s";
            CMacAddrUniqueCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(mac).c_str());
        }
        else
        {
            setMac.insert(mac);
        }
    }
    return true;
}

// SCD�ļ���IED��IP��ַ�Ƿ�ȫ��Ψһ
bool CCacheMgr::IpUniqueCheck(wstring &ip)
{
    if (CIpUniqueCheck::GetInstance()->IsEnabled())
    {
        if (ip.size() == 0)
        {
            return true;
        }
        if (setIP.count(ip) == 1)
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "IP��ַ�����ظ�:IP value=%s";
            CIpUniqueCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(ip).c_str());
        }
        else
        {
            setIP.insert(ip);
        }
    }
    return true;
}

// SCD�ļ���GSEControl��appID��ֵ��SampledValueControl��smvID��ֵ�Ƿ�ȫ��ΨһReportControl��rptID��ֵ�Ƿ�ȫ��Ψһ��
bool CCacheMgr::ControlIdUniqueCheck(wstring &id)
{
    if (CControlIdUniqueCheck::GetInstance()->IsEnabled())
    {
        if (id.size() ==  0)
        {
            return true;
        }
        if (setCBId.count(id) == 1)
        {
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            string msg = "���ƿ��appID��smvID��Ψһ: ID=%s";
            CControlIdUniqueCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(id).c_str());
        }
        else 
        {
            setCBId.insert(id);
        }
    }
    return true;
}

// GOOSE ��SV���õ�ExtRef�ڲ�intAddr��ֵ��IED��Χ���Ƿ�Ψһ��
bool CCacheMgr::IntAddrUniqueCheck(LDCache &ldCache, wstring &intAddr)
{
    if (CIntAddrUniqueCheck::GetInstance()->IsEnabled())
    {
        if (intAddr.size() == 0)
        {
            return true;
        }
        
        if (setIntAddr.count(intAddr) == 1)
        {
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            string msg = "intAddr��IED�ڲ�Ψһ:IED name=%s LDevice inst=%s intAddr=%s";
            CIntAddrUniqueCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(ldCache.iedName).c_str(),
                WtoA(ldCache.inst).c_str(),
                WtoA(intAddr).c_str());
        }
        else
        {
            setIntAddr.insert(intAddr);
        }
    }
    return true;
}


// Communication��SubNetwork�����������Ƿ���Ϲ淶�е�����Ҫ��
bool CCacheMgr::SubFormatCheck(wstring &subNetWorkName)
{
    if (CSubFormatCheck::GetInstance()->IsEnabled())
    {
        if ((subNetWorkName.find(L"Subnetwork_Stationbus") == wstring::npos) &&
            (subNetWorkName.find(L"Subnetwork_Processbus") == wstring::npos))
        {
            string msg = "Communication��SubNetwork���������������Ϲ淶�е�����Ҫ��, \
                ȫվ�����˻��ֳ�վ�ز�͹��̲����������������ֱ�Ϊ��Subnetwork_Stationbus���͡�Subnetwork_Processbus��:SubNetwork name=%s";
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSubFormatCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(subNetWorkName).c_str());
        }
    }
    return true;
}

// GOOSE ��SV���õ�ExtRef�ڲ�intAddr��ֵ�Ƿ���Ϲ淶�е�������ʽҪ�󣬹淶��ʽӦΪ��LD/LN.DO.DA��(Q/GDW 396-2009)��n-A:LD/LN.DO.DA����Q/GDW 396-2012����
bool CCacheMgr::InputsFormatCheck(LDCache &ldCache, wstring &intAddr)
{
    if (CInputsFormatCheck::GetInstance()->IsEnabled())
    {
        bool bFormatError = false;
        // �̵�ַΪ��
        if (intAddr.size() == 0)
        {
            bFormatError = true;
        }
        // ����˿�Ϊ��
        int pos = intAddr.find(L":");
        if (pos != wstring::npos && pos == 0)
        {
            bFormatError = true;
        }
        // ��ֻ��LNҲΪ��
        if (intAddr.find(L".") == wstring::npos)
        {
            bFormatError = true;
        }
        // �������Ƿ��ַ�
        string temp = WtoA(intAddr);
        for (int i = 0; i < temp.size(); i++)
        {
            char c = intAddr.at(i);
            // �ַ���"A-Z"��"a-z"��"��"��"/"����-������_����
            if ((c < '-') || ((c > ':') && (c < 'A')) || ((c > 'Z') && (c < '_')) || ((c > '_') && (c < 'a')) || (c > 'z'))
            {
                bFormatError = true;
                break;
            }
        }

        if (bFormatError)
        {
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            string msg = "ExtRef�ڲ�intAddr��ֵ�����Ƿ��ַ�:IED name=%s LDevice inst=%s �ڲ���ַintAddr=%s";
            CInputsFormatCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(ldCache.iedName).c_str(),
                WtoA(ldCache.inst).c_str(),
                WtoA(intAddr).c_str());
        }
    }
    return true;
}



// ----------------------------------------------------------------------------------private function-----------------------------------------------------------------------------------

// Get the node xml
wstring CCacheMgr::GetXMLContent()
{
    Long Pos = m_vn->getElementFragment();

    long length = (Pos & 0xFFFFFFFF00000000)>>32;
    long offset = (Pos & 0xFFFFFFFF);

    UByte* pDoc = m_vn->getXML();

    string str1(pDoc + offset, pDoc+offset+length);
    return AtoW(str1);
}

long CCacheMgr::GetOffsetOfVTD()
{
    Long Pos = m_vn->getElementFragment();
    return (Pos & 0xFFFFFFFF);
}


void CCacheMgr::ReportCheckResult(const wstring &message, const wstring &xml)
{
    wprintf(L"\n%s, %s\n", message.c_str(), xml.c_str());
}

//----------------------------------------------------------------------------------------------------------Substation-------------------------------------------------------------

// SSD SubStation
bool CCacheMgr::GetSSDSubstationCache()
{
    try
    {
        m_vn->toElement(ROOT);

        int iSizeOfSubstation = 0; //��¼Substation���� 
        int offset = GetOffsetOfVTD(); 
        string name; // ��¼���վ����

        if (m_vn->toElement(FIRST_CHILD, L"Substation"))
        {
                
            do 
            {
                iSizeOfSubstation++;
                if (iSizeOfSubstation > 1) // ��ֹ���ݱ���
                {
                    break;
                }

                int  iSubstationOffset = GetOffsetOfVTD();       
                // �ж������Ƿ�ѡ��
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Attr_R").bEnabled;
                ErrorType etSSDSubstationCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Attr_R").etValue;

                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Attr_O").bEnabled;
                ErrorType etSSDSubstationCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Attr_O").etValue;

                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Child").bEnabled;
                ErrorType etSSDSubstationCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Child").etValue;

                bool bParentChecked = CSSDSubstationCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 1); // 1�����ԣ�name
                AddDueElementInfo(bParentChecked, bAttr_O, 4); // 4�����ԣ�desc��dtype��area��areaDesc
                AddDueElementInfo(bParentChecked, bChild, 1); // 1����Ԫ�أ�VoltageLevel

                name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                // ���Ի�ȡ�����
                if (m_vn->hasAttr(L"name"))
                {
                    m_SCDCache.ssd.name = VtdGetAttributeValue(m_vn, L"name");
                }
                else
                {
                    string msg = FormatXMLMessage("[Substation name=%s]SubstationԪ��ȱ��name����", name.c_str());
                    SSDSubstationCheck(name, msg, bAttr_R, etSSDSubstationCheck_Attr_R);
                }
                if (m_vn->hasAttr(L"desc"))
                {
                    m_SCDCache.ssd.desc = VtdGetAttributeValue(m_vn, L"desc");
                }
                else
                {
                    string msg = FormatXMLMessage("[Substation name=%s]SubstationԪ��ȱ��desc����", name.c_str());
                    SSDSubstationCheck(name, msg, bAttr_O, etSSDSubstationCheck_Attr_O);
                }

                // ��¼�Ƿ���Voltagelevel��Private����
                bool bHaveVoltageLevel = false;
                bool bHaveDType = false;
                bool bHaveAreaName = false;
                bool bHaveAreaDesc = false;

                // ������Ԫ��
                if (m_vn->toElement(FIRST_CHILD))
                {
                    do 
                    {         
                        wstring tag = VtdGetElementName(m_vn);
                        if (tag == L"VoltageLevel")
                        {
                            bHaveVoltageLevel = true;
                            GetSSDVoltageLevelCache(m_SCDCache.ssd);
                        }
                        else if (tag == L"Private")
                        {
                            //Private
                            wstring ptype = VtdGetAttributeValue(m_vn, L"type");
                            if (m_vn->hasAttr(L"ext:dType"))
                            {
                                bHaveDType = true;
                                m_SCDCache.ssd.dtype = VtdGetAttributeValue(m_vn, L"ext:dType");
                                SSDSubstationCheck_NormativeCheck(name, name, WtoA(m_SCDCache.ssd.dtype));
                            }
                            if (m_vn->hasAttr(L"ext:areaname"))
                            {
                                bHaveAreaName = true;
                                m_SCDCache.ssd.areaName = VtdGetAttributeValue(m_vn, L"ext:areaname");
                            }
                            if (m_vn->hasAttr(L"ext:desc"))
                            {
                                bHaveAreaDesc = true;
                                m_SCDCache.ssd.areaDesc = VtdGetAttributeValue(m_vn, L"ext:desc");
                            }
                        }
                        else {
                            // SubStation�Ƿ�������Ԫ�أ�LNode�ȣ�
                            SSDSubstationCheck_OtherCheck(name, name, WtoA(tag));
                        }

                    } while (m_vn->toElement(NEXT_SIBLING));

                    // ���ظ�Ԫ��
                    m_vn->toElement(PARENT);
                }

                // ��ѹ�ȼ��Ƿ��մӸߵ��͵�˳���������
                std::vector<int> vtgLevelVector;
                for (int i = 0; i < m_SCDCache.ssd.vVoltageLevel.size(); i++)
                {
                    string name = WtoA(m_SCDCache.ssd.vVoltageLevel.at(i).name);
                    int ord = CConfigMgr::GetInstance()->IsInVoltageLevelTypeMap(name);

                    bool bError = false;
                    for (int j = 0; j < vtgLevelVector.size(); j++)
                    {
                        if (ord > vtgLevelVector.at(j))
                        {
                            bError = true;
                            SSDVoltageLevelCheck_SortCheck(WtoA(m_SCDCache.ssd.name), WtoA(m_SCDCache.ssd.name), iSubstationOffset);
                            break;
                        }
                    }
                    if (bError)
                    {
                        break;
                    }
                    vtgLevelVector.push_back(ord);
                }

                // ���������Ԫ�غ��ж��Ƿ���VoltageLevel��dtype��area
                if (!bHaveVoltageLevel)
                {
                    string msg = FormatXMLMessage("[Substation name=%s]SubstationԪ��ȱ��VoltageLevel��Ԫ��", name.c_str());
                    SSDSubstationCheck(name, msg, bChild, etSSDSubstationCheck_Child);
                }
                if (!bHaveDType)
                {
                    string msg = FormatXMLMessage("[Substation name=%s]SubstationԪ��ȱ��dtype����", name.c_str());
                    SSDSubstationCheck(name, msg, bAttr_O, etSSDSubstationCheck_Attr_O);
                }
                if (!bHaveAreaName)
                {
                    string msg = FormatXMLMessage("[Substation name=%s]SubstationԪ��ȱ��area��name����", name.c_str());
                    SSDSubstationCheck(name, msg, bAttr_O, etSSDSubstationCheck_Attr_O);
                }
                if (!bHaveAreaDesc)
                {
                    string msg = FormatXMLMessage("[Substation name=%s]SubstationԪ��ȱ��area��desc����", name.c_str());
                    SSDSubstationCheck(name, msg, bAttr_O, etSSDSubstationCheck_Attr_O);
                }

            } while (m_vn->toElement(NEXT_SIBLING, L"Substation"));
        
           
        }
        // �ж�Substation�Ƿ�Ψһ
        if (CSSDSubstationCheck_Unique::GetInstance()->IsEnabled())
        {
            int row = pChecker->CalcRowNumberFromOffset(offset);
            string xml = pChecker->GetRowContentByOffset(offset);
            if (iSizeOfSubstation > 1)
            {
                std::string message = "�ļ��д��ڶ��SubstationԪ��";
                CSSDSubstationCheck_Unique::GetInstance()->Report(name, CSSDSubstationCheck_Unique::GetInstance()->GetErrorType(), row, xml, message.c_str());
            }
            else if (iSizeOfSubstation == 0)
            {
                std::string message = "�ļ��в�����SubstationԪ��";
                CSSDSubstationCheck_Unique::GetInstance()->Report(name, CSSDSubstationCheck_Unique::GetInstance()->GetErrorType(), row, xml, message.c_str());
            }
        }

        // �ж��Ƿ���ConnectivityNodeδ��ʹ��
        SSDConnectivityNodeCheck_UnUsedCheck();
        // Terminal��SubstationName��VoltageLevelName��bayName��cNodeName��connectivityNode�����Ƿ���Ч
        SSDTerminalCheck_ValidCheck();
    }
    catch(...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetSSDVoltageLevelCache(SubstationCache &substation)
{
    try
    {
        VoltageLevelCache voltagelevel;

        // �ж������Ƿ�ѡ��
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDVoltageLevelCheck_Attr_R").bEnabled;
        ErrorType etSSDVoltageLevelCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDVoltageLevelCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDVoltageLevelCheck_Attr_O").bEnabled;
        ErrorType etSSDVoltageLevelCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDVoltageLevelCheck_Attr_O").etValue;

        bool bChild_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDVoltageLevelCheck_Child_R").bEnabled;
        ErrorType etSSDVoltageLevelCheck_Child_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDVoltageLevelCheck_Child_R").etValue;

        bool bChild_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDVoltageLevelCheck_Child_O").bEnabled;
        ErrorType etSSDVoltageLevelCheck_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDVoltageLevelCheck_Child_O").etValue;

        bool bVoltageAttr = CConfigMgr::GetInstance()->GetSubItemConfig("SSDVoltageLevelCheck_Voltage_Attr").bEnabled; 
        ErrorType etSSDVoltageLevelCheck_Voltage_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SSDVoltageLevelCheck_Voltage_Attr").etValue;

        bool bParentChecked = CSSDVoltageLevelCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 1); // 2�����ԣ�name��desc
        AddDueElementInfo(bParentChecked, bChild_R, 1); // 2����Ԫ�أ�Voltage��Bay
        AddDueElementInfo(bParentChecked, bAttr_O, 1); // 2�����ԣ�name��desc
        AddDueElementInfo(bParentChecked, bChild_O, 1); // 2����Ԫ�أ�Voltage��Bay
        
        string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
        string key = WtoA(substation.name) + "/" + name;
        voltagelevel.key = AtoW(key);
        if (m_vn->hasAttr(L"name"))
        {
            voltagelevel.name = VtdGetAttributeValue(m_vn, L"name");
        }
        else
        {
            string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevelȱ��name����", WtoA(substation.name).c_str(), name.c_str());
            SSDVoltageLevelCheck(key, msg, bAttr_R, etSSDVoltageLevelCheck_Attr_R);
        }
        if (m_vn->hasAttr(L"desc"))
        {
            voltagelevel.desc = VtdGetAttributeValue(m_vn, L"desc");
        } 
        else
        {
            string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevelȱ��desc����", WtoA(substation.name).c_str(), name.c_str());
            SSDVoltageLevelCheck(key, msg, bAttr_O, etSSDVoltageLevelCheck_Attr_O);
        }

        // VoltageLevel��name��desc�����Ƿ���϶��ѹ��׼
        SSDVoltageLevelCheck_NormativeCheck(key, WtoA(substation.name), WtoA(voltagelevel.name), true);
        SSDVoltageLevelCheck_NormativeCheck(key, WtoA(substation.name), WtoA(voltagelevel.desc), false);

        // ����Ϣ����Substation�ṹ
        for (int j = 0; j < substation.vVoltageLevel.size(); j++)
        {
            if (substation.vVoltageLevel.at(j).name == voltagelevel.name)
            {
                string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevel��Ψһ", WtoA(substation.name).c_str(), name.c_str());
                SSDVoltageLevelCheck_UniqueCheck(key, msg);
            }
        }

        // ��¼�Ƿ���Bay��Voltage
        bool bHaveBay = false;
        bool bHaveVoltage = false;
        bool bHaveMultiplier = false;
        bool bHaveUnit = false;

        // ������Ԫ��
        if (m_vn->toElement(FIRST_CHILD))
        {
            do 
            {
                wstring tag = VtdGetElementName(m_vn);
                if (tag == L"Bay")
                {
                    bHaveBay = true;
                    GetSSDBayCache(voltagelevel);
                }
                else if (tag == L"Voltage")
                {
                    AddDueElementInfo(bParentChecked, bVoltageAttr, 2); // Voltage 2�����ԣ�name��desc

                    bHaveVoltage = true;

                    // ���multiplier����
                    if (m_vn->hasAttr(L"multiplier"))
                    {
                        bHaveMultiplier = true;
                        voltagelevel.voltage.multiplier = VtdGetAttributeValue(m_vn, L"multiplier");
                        if (voltagelevel.voltage.multiplier != L"k")
                        {
                            //TODO
                        }
                    }
                    else
                    {
                        string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevelȱ��multiplier����", WtoA(substation.name).c_str(), name.c_str());
                        SSDVoltageLevelCheck(key, msg, bVoltageAttr, etSSDVoltageLevelCheck_Voltage_Attr);
                    }

                    // ���unit����
                    if (m_vn->hasAttr(L"unit"))
                    {
                        bHaveUnit = true;
                        voltagelevel.voltage.unit = VtdGetAttributeValue(m_vn, L"unit");
                        if (voltagelevel.voltage.unit != L"V")
                        {
                            //TODO
                        }
                    }
                    else
                    {
                        string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevelȱ��unit����", WtoA(substation.name).c_str(), name.c_str());
                        SSDVoltageLevelCheck(key, msg, bVoltageAttr, etSSDVoltageLevelCheck_Voltage_Attr);
                    }

                    voltagelevel.voltage.text = VtdGetTextValue(m_vn);
                    // VoltageLevelΪ��/�ǿ�ʱ��Voltageֵ�Ƿ�Ϊ�շǿ�
                    wstring voltage = voltagelevel.voltage.text + voltagelevel.voltage.multiplier + voltagelevel.voltage.unit;
                    if (!((voltagelevel.name == L"" && voltagelevel.voltage.text == L"") ||
                        (voltagelevel.name != L"" && voltage == voltagelevel.name)))
                    {
                        string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s Voltage text=%s]VoltageLevelֵ��Voltage��һ��", WtoA(substation.name).c_str(), WtoA(voltagelevel.name).c_str(), WtoA(voltage).c_str());
                        SSDVoltageLevelCheck_UnNULLCheck(key, msg);
                    }
                }
                else
                {
                    // VoltageLevel�Ƿ�������Ԫ�أ�LNode�ȣ�
                    SSDVoltageLevelCheck_OtherCheck(key, WtoA(substation.name), name, WtoA(tag));
                }
            } while (m_vn->toElement(NEXT_SIBLING));

            // ���ظ�Ԫ��
            m_vn->toElement(PARENT);
        }

        if (!bHaveBay)
        {
            string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevelȱ��Bay��Ԫ��", WtoA(substation.name).c_str(), name.c_str());
            SSDVoltageLevelCheck(key, msg, bChild_R, etSSDVoltageLevelCheck_Child_R);
        }
        if (!bHaveVoltage)
        {
            string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevelȱ��Voltage��Ԫ��", WtoA(substation.name).c_str(), name.c_str());
            SSDVoltageLevelCheck(key, msg, bChild_O, etSSDVoltageLevelCheck_Child_O);
        }
        substation.vVoltageLevel.push_back(voltagelevel);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetSSDBayCache(VoltageLevelCache &voltageLevel)
{
    try
    {
        // �ж������Ƿ�ѡ��
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Attr_R").bEnabled;
        ErrorType etSSDBayCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Attr_O").bEnabled;
        ErrorType etSSDBayCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Attr_O").etValue;

        bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Child").bEnabled;
        ErrorType etSSDBayCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Child").etValue;

        // bool bVirtual = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Virtual").bEnabled;
        bool bParentChecked = CSSDBayCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 1); // 2�����ԣ�name
        AddDueElementInfo(bParentChecked, bAttr_O, 2); // 2�����ԣ�desc��dtype
        AddDueElementInfo(bParentChecked, bChild, 1); // 1����Ԫ�أ�PowerTransformer��ConductingEquipment��ConnectivityNode�� LNode
        // AddDueElementInfo(bParentChecked, bVirtual, 1); // Virtual 2����Ԫ�أ�Function��LNode

        string bayName = WtoA(VtdGetAttributeValue(m_vn, L"name"));
        BayCache bay;
        string key = WtoA(voltageLevel.key) + "/" + bayName;
        bay.key = AtoW(key);
        if (m_vn->hasAttr(L"name"))
        {
            bay.name = VtdGetAttributeValue(m_vn, L"name");
        }
        else
        {
            string msg = FormatXMLMessage("[VoltageLevel name=%s Bay name=%s]BayԪ��ȱ��name����", WtoA(voltageLevel.name).c_str(), bayName.c_str());
            SSDBayCheck(key, msg, bAttr_R, etSSDBayCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            bay.desc = VtdGetAttributeValue(m_vn, L"desc");
        }
        else
        {
            string msg = FormatXMLMessage("[VoltageLevel name=%s Bay name=%s]BayԪ��ȱ��desc����", WtoA(voltageLevel.name).c_str(), bayName.c_str());
            SSDBayCheck(key, msg, bAttr_O, etSSDBayCheck_Attr_O);
        }

        // have dType?
        bool bHaveDType = false;
        bool bHaveChild = false;
        bool bHaveVirtualChild = false;

        string formatedMessage = FormatXMLMessage("VoltageLevel name=%s, Bay name=%s", WtoA(voltageLevel.name).c_str(), bayName.c_str());

        if (m_vn->toElement(FIRST_CHILD))
        {
            do 
            {
                wstring tag = VtdGetElementName(m_vn);
                if (tag == L"Private")
                {
                    //Private
                    if (m_vn->hasAttr(L"ext:dType"))
                    {
                        bHaveDType = true;
                        bay.dtype = VtdGetAttributeValue(m_vn, L"ext:dType");
                        SSDBayCheck_dTypeNormativeCheck(key, WtoA(voltageLevel.name), bayName, WtoA(bay.dtype));
                    }
                } 
                else if (tag == L"PowerTransformer")
                {
                    bHaveChild = true;
                    GetSSDPowerTransformerCache(key, formatedMessage, bay.ptMap);
                }
                else if (tag == L"ConductingEquipment")
                {
                    bHaveChild = true;
                    GetSSDConductingEquipmentCache(key, formatedMessage, bay.ceMap);
                }
                else if (tag == L"ConnectivityNode")
                {
                    bHaveChild = true;
                    GetSSDConnectivityNodeCache(key, formatedMessage, bay.cnMap);
                }
                else if (tag == L"LNode")
                {
                    bHaveChild = true;
                    bHaveVirtualChild = true;
                    GetSSDLNodeCache(key, formatedMessage, bay.lnMap);
                }
                else if (tag == L"Function")
                {
                    bHaveVirtualChild = true;
                    GetSSDFunctionCache(key, formatedMessage, bay.funcMap);
                }
                else
                {
                    //TODO
                    wstring msg = L"���зǷ���Ԫ��";
                }

            } while (m_vn->toElement(NEXT_SIBLING));

            // ���ظ�Ԫ��
            m_vn->toElement(PARENT);
        }

        // һ���豸���BayԪ�ص�name��desc�����Ƿ���Ϲ淶
        SSDBayCheck_AttrNormativeCheck(key, formatedMessage, bay);
        if (bay.ptMap.size() > 1)
        {
             SSDPowerTransformerCheck_UniqueCheck(key, formatedMessage);
        }       

        if (!bHaveDType)
        {
            string msg = FormatXMLMessage("[VoltageLevel name=%s Bay name=%s]BayԪ��ȱ��dType����", WtoA(voltageLevel.name).c_str(), bayName.c_str());
            SSDBayCheck(key, msg, bAttr_O, etSSDBayCheck_Attr_O);
        }
        if (!bHaveChild)
        {
            string msg = FormatXMLMessage("[VoltageLevel name=%s Bay name=%s]BayԪ��ȱ��PowerTransformer��ConductingEquipment��ConnectivityNode��LNode�е��κ�һ��", WtoA(voltageLevel.name).c_str(), bayName.c_str());
            SSDBayCheck(key, msg, bChild, etSSDBayCheck_Child);
        }
        //if (!bHaveVirtualChild)
        //{
        //    string msg = "����BayԪ��ȱ��Function��LNode�е��κ�һ��";
        //    SSDBayCheck(msg, bVirtual);
        //}

        voltageLevel.bayMap.insert(make_pair(bay.name, bay));
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CCacheMgr::GetSSDPowerTransformerCache(string key, string &formatedPath, PowerTransformerCacheMap &ptMap)
{
    try
    {
        // �ж������Ƿ�ѡ��
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Attr_R").bEnabled;
        ErrorType etSSDPowerTransformerCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Attr_O").bEnabled;
        ErrorType etSSDPowerTransformerCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Attr_O").etValue;

        bool bChild_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Child_R").bEnabled;
        ErrorType etSSDPowerTransformerCheck_Child_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Child_R").etValue;

        bool bChild_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Child_O").bEnabled;
        ErrorType etSSDPowerTransformerCheck_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Child_O").etValue;

        bool bParentChecked = CSSDPowerTransformerCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 2); // 4�����ԣ�name��desc
        AddDueElementInfo(bParentChecked, bAttr_O, 2); // 4�����ԣ�name��desc

        AddDueElementInfo(bParentChecked, bChild_R, 1); // 2����Ԫ�أ�LNode��TransformerWinding
        AddDueElementInfo(bParentChecked, bChild_O, 1); // 2����Ԫ�أ�LNode��TransformerWinding

        string ptName = WtoA(VtdGetAttributeValue(m_vn, L"name"));
        PowerTransformerCache pt;
        string newKey = key + "/" + ptName;
        pt.key = AtoW(newKey);
        if (m_vn->hasAttr(L"name"))
        {
            pt.name = VtdGetAttributeValue(m_vn, L"name");
            SSDPowerTransformerCheck_AttrNormativeCheck(newKey, formatedPath, ptName.c_str());
        }
        else
        {
            string msg = FormatXMLMessage("[%s, PowerTransformer name=%s]PowerTransformerԪ��ȱ��name����", formatedPath.c_str(), ptName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bAttr_R, etSSDPowerTransformerCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            pt.desc = VtdGetAttributeValue(m_vn, L"desc");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, PowerTransformer name=%s]PowerTransformerԪ��ȱ��desc����", formatedPath.c_str(), ptName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bAttr_O, etSSDPowerTransformerCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"virtual"))
        {
            pt.vir = VtdGetAttributeValue(m_vn, L"virtual");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, PowerTransformer name=%s]PowerTransformerԪ��ȱ��virtual����", formatedPath.c_str(), ptName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bAttr_O, etSSDPowerTransformerCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"type"))
        {
            pt.type = VtdGetAttributeValue(m_vn, L"type");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, PowerTransformer name=%s]PowerTransformerԪ��ȱ��type����", formatedPath.c_str(), ptName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bAttr_R, etSSDPowerTransformerCheck_Attr_R);
        }

        // bHaveLnode\TransformWinding
        bool bHaveLNode = false;
        bool bHaveTransformerWinding = false;

        string formatedMessage = FormatXMLMessage("%s, PowerTransformer name=%s", formatedPath.c_str(), ptName.c_str());
        if (m_vn->toElement(FIRST_CHILD))
        {
            do 
            {
                wstring tag = VtdGetElementName(m_vn);
                if (tag == L"LNode")
                {
                    bHaveLNode = true;
                    GetSSDLNodeCache(newKey, formatedMessage, pt.lnodeMap);
                }
                else if (tag == L"TransformerWinding")
                {
                    bHaveTransformerWinding = true;
                    GetSSDTransformerWindingCache(newKey, formatedMessage, pt.tfWindingMap);
                }
                else
                {
                    //TODO
                }
            } while (m_vn->toElement(NEXT_SIBLING));

            // ���ظ�Ԫ��
            m_vn->toElement(PARENT);
        }
        
        if (pt.tfWindingMap.size() < 2)
        {
            SSDPowerTransformerCheck_WindingNormativeCheck(newKey, formatedMessage);
        }
        
        if (!bHaveTransformerWinding)
        {
            string msg = FormatXMLMessage("[%s]PowerTransformerԪ��ȱ��TransformerWinding��Ԫ��", formatedMessage.c_str());
            SSDPowerTransformerCheck(newKey, msg, bChild_R, etSSDPowerTransformerCheck_Child_R);
        }

        if (!bHaveLNode)
        {
            string msg = FormatXMLMessage("[%s]PowerTransformerԪ��ȱ��LNode��Ԫ��", formatedMessage.c_str());
            SSDPowerTransformerCheck(newKey, msg, bChild_O, etSSDPowerTransformerCheck_Child_O);
        }

        ptMap.insert(make_pair(pt.name, pt));
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CCacheMgr::GetSSDTransformerWindingCache(string key, string &formatedPath, TransformerWindingCacheMap &twMap)
{
    try
    {
        // �ж������Ƿ�ѡ��
        bool bTfWindingAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_TFWinding_Attr_R").bEnabled;
        ErrorType etSSDPowerTransformerCheck_TFWinding_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_TFWinding_Attr_R").etValue;

        bool bTfWindingAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_TFWinding_Attr_O").bEnabled;
        ErrorType etSSDPowerTransformerCheck_TFWinding_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_TFWinding_Attr_O").etValue;

        // bool bTfWindingChild = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_TFWinding_Child").bEnabled;
        bool bParentChecked = CSSDPowerTransformerCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bTfWindingAttr_R, 2); // 4������
        AddDueElementInfo(bParentChecked, bTfWindingAttr_O, 2); // 4������
        // AddDueElementInfo(bParentChecked, bTfWindingChild, 1); // 1����Ԫ�أ���LNode��,��Terminal��,��SubEquipment��,��TapChanger������Ϊ��ѡ

        string transformWindingName = WtoA(VtdGetAttributeValue(m_vn, L"name"));
        TransformerWindingCache tw;
        string newKey = key + "/" + transformWindingName;
        tw.key = AtoW(newKey);
        if (m_vn->hasAttr(L"name"))
        {
            tw.name = VtdGetAttributeValue(m_vn, L"name");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, TransformerWinding name=%s]TransformerWindingԪ��ȱ��name����", formatedPath.c_str(), transformWindingName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bTfWindingAttr_R, etSSDPowerTransformerCheck_TFWinding_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            tw.desc = VtdGetAttributeValue(m_vn, L"desc");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, TransformerWinding name=%s]TransformerWindingԪ��ȱ��desc����", formatedPath.c_str(), transformWindingName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bTfWindingAttr_O, etSSDPowerTransformerCheck_TFWinding_Attr_O);
        }

        if (m_vn->hasAttr(L"virtual"))
        {
            tw.vir = VtdGetAttributeValue(m_vn, L"virtual");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, TransformerWinding name=%s]TransformerWindingԪ��ȱ��virtual����", formatedPath.c_str(), transformWindingName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bTfWindingAttr_O, etSSDPowerTransformerCheck_TFWinding_Attr_O);
        }

        if (m_vn->hasAttr(L"type"))
        {
            tw.type = VtdGetAttributeValue(m_vn, L"type");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, TransformerWinding name=%s]TransformerWindingԪ��ȱ��type����", formatedPath.c_str(), transformWindingName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bTfWindingAttr_R, etSSDPowerTransformerCheck_TFWinding_Attr_R);
        }

        // bHavaChild
        bool bHavaChild = false;

        string formatedMessage = FormatXMLMessage("%s, TransformerWinding name=%s", formatedPath.c_str(), transformWindingName.c_str());
        if (m_vn->toElement(FIRST_CHILD))
        {
            do 
            {
                wstring tag = VtdGetElementName(m_vn);
                if (tag == L"Terminal")
                {
                    bHavaChild = true;
                    GetSSDTerminalCache(newKey, formatedMessage, tw.terminalMap);
                }
                else if (tag == L"LNode")
                {
                    bHavaChild = true;
                    GetSSDLNodeCache(newKey, formatedMessage, tw.lnMap);
                }
                else if (tag == L"SubEquipment")
                {
                    // TODO
                    // bHavaChild = true;
                    // GetSSDCommonEquipmentCache(tw.subEquipmentMap);
                }
                else if (tag == L"TapChanger")
                {
                    // TODO
                    /*bHavaChild = true;
                    GetSSDCommonEquipmentCache(newKey, formatedMessage, tw.tapChangerMap);*/
                }
                else
                {
                    //TODO
                }

            } while (m_vn->toElement(NEXT_SIBLING));

            // ���ظ�Ԫ��
            m_vn->toElement(PARENT);
        }

        //if (!bHavaChild)
        //{
        //    string msg = "TransformerWindingԪ��ȱ����Ԫ��";
        //    SSDPowerTransformerCheck(msg, bTfWindingChild);
        //}

        twMap.insert(make_pair(tw.name, tw));
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetSSDConductingEquipmentCache(string key, string &formatedPath, ConductingEquipmentCacheMap &eqipmentMap)
{
    try
    {
        // �ж������Ƿ�ѡ��
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConductingEquipmentCheck_Attr_R").bEnabled;
        ErrorType etSSDConductingEquipmentCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConductingEquipmentCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConductingEquipmentCheck_Attr_O").bEnabled;
        ErrorType etSSDConductingEquipmentCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConductingEquipmentCheck_Attr_O").etValue;
        // bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConductingEquipmentCheck_Child").bEnabled;
        bool bParentChecked = CSSDConductingEquipmentCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 2); // 2�����ԣ�name��desc
        AddDueElementInfo(bParentChecked, bAttr_O, 2); // 2�����ԣ�name��desc
        // AddDueElementInfo(bParentChecked, bChild, 1); // ��ѡ��Ԫ��

        string ceName = WtoA(VtdGetAttributeValue(m_vn, L"name"));
        ConductingEquipmentCache equipment;
        string newKey = key + "/" + ceName;
        equipment.key = AtoW(newKey);
        equipment.offset = GetOffsetOfVTD();
        if (m_vn->hasAttr(L"name"))
        {
            equipment.name = VtdGetAttributeValue(m_vn, L"name");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, ConductingEquipment name=%s]ConductingEquipmentԪ��ȱ��name����", formatedPath.c_str(), ceName.c_str());
            SSDConductingEquipmentCheck(newKey, msg, bAttr_R, etSSDConductingEquipmentCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            equipment.desc = VtdGetAttributeValue(m_vn, L"desc");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, ConductingEquipment name=%s]ConductingEquipmentԪ��ȱ��desc����", formatedPath.c_str(), ceName.c_str());
            SSDConductingEquipmentCheck(newKey, msg, bAttr_O, etSSDConductingEquipmentCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"virtual"))
        {
            equipment.vir = VtdGetAttributeValue(m_vn, L"virtual");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, ConductingEquipment name=%s]ConductingEquipmentԪ��ȱ��virtual����", formatedPath.c_str(), ceName.c_str());
            SSDConductingEquipmentCheck(newKey, msg, bAttr_O, etSSDConductingEquipmentCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"type"))
        {
            equipment.type = VtdGetAttributeValue(m_vn, L"type");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, ConductingEquipment name=%s]ConductingEquipmentԪ��ȱ��type����", formatedPath.c_str(), ceName.c_str());
            SSDConductingEquipmentCheck(newKey, msg, bAttr_R, etSSDConductingEquipmentCheck_Attr_R);
        }

        //bHaveChild
        bool bHaveChild = false;
        string formatedMessage = FormatXMLMessage("%s, ConductingEquipment name=%s", formatedPath.c_str(), ceName.c_str());
        equipment.path = AtoW(formatedMessage);
        if (m_vn->toElement(FIRST_CHILD))
        {
            do 
            {
                wstring tag = VtdGetElementName(m_vn);
                if (tag == L"LNode")
                {
                    bHaveChild = true;
                    GetSSDLNodeCache(newKey, formatedMessage, equipment.lnMap);
                }
                else if (tag == L"Terminal")
                {
                    bHaveChild = true;
                    GetSSDTerminalCache(newKey, formatedMessage, equipment.terminalMap);
                }
                else if (tag == L"subEquipment")
                {
                    bHaveChild = true;
                    // TODO
                    // GetSSDCommonEquipmentCache(equipment.subEquipmentMap);
                }
                else
                {
                    //TODO
                }

            } while (m_vn->toElement(NEXT_SIBLING));

            // ���ظ�Ԫ��
            m_vn->toElement(PARENT);
        }

        // �ж��Ƿ�Ϊ�������豸
        if (CSSDConductingEquipmentCheck_IFLEquipment::GetInstance()->IsEnabled() &&
            (equipment.type.find(L"LIN") != wstring::npos) ||
            (equipment.type.find(L"IFL") != wstring::npos))
        {
            SSDConductingEquipmentCheck_IFLEquipmentCheck(newKey, equipment);
        }

        if (eqipmentMap.find(equipment.name) != eqipmentMap.end())
        {
            SSDConductingEquipmentCheck_UniqueCheck(newKey, formatedMessage);
        }
        else
        {
            eqipmentMap.insert(make_pair(equipment.name, equipment));
        }
        
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetSSDLNodeCache(string parentKey, string &formatedPath, LNodeCacheMap &lnMap)
{
    try
    {
        // �ж������Ƿ�ѡ��
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDLNodeCheck_Attr_R").bEnabled;
        ErrorType etSSDLNodeCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDLNodeCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDLNodeCheck_Attr_O").bEnabled;
        ErrorType etSSDLNodeCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDLNodeCheck_Attr_O").etValue;

        bool bParentChecked = CSSDLNodeCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 4); // 4�����ԣ�lnInst��lnClass��iedName��ldInst
        AddDueElementInfo(bParentChecked, bAttr_O, 2); // 2�����ԣ�prefix��lnType��

        LNodeCache ln;
        ln.iedName = VtdGetAttributeValue(m_vn, L"iedName");
        ln.ldInst = VtdGetAttributeValue(m_vn, L"ldInst");
        ln.lnType = VtdGetAttributeValue(m_vn, L"lnType");
        ln.lnInst = VtdGetAttributeValue(m_vn, L"lnInst");
        ln.lnClass = VtdGetAttributeValue(m_vn, L"lnClass");
        ln.prefix = VtdGetAttributeValue(m_vn, L"prefix");
        wstring key = ln.iedName + L"." + ln.ldInst + L"/" + ln.prefix + ln.lnClass + ln.lnInst;
        ln.key = key;

        if (!m_vn->hasAttr(L"iedName"))
        {
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNodeԪ��ȱ��iedName����", formatedPath.c_str(), WtoA(key).c_str());
            SSDLNodeCheck(WtoA(key), msg, bAttr_R, etSSDLNodeCheck_Attr_R);
        }
        if (!m_vn->hasAttr(L"ldInst"))
        {
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNodeԪ��ȱ��ldInst����", formatedPath.c_str(), WtoA(key).c_str());
            SSDLNodeCheck(WtoA(key), msg, bAttr_R, etSSDLNodeCheck_Attr_R);
        }
        if (!m_vn->hasAttr(L"lnInst"))
        {
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNodeԪ��ȱ��lnInst����", formatedPath.c_str(), WtoA(key).c_str());
            SSDLNodeCheck(WtoA(key), msg, bAttr_R, etSSDLNodeCheck_Attr_R);
        }
        if (!m_vn->hasAttr(L"lnClass"))
        {
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNodeԪ��ȱ��lnClass����", formatedPath.c_str(), WtoA(key).c_str());
            SSDLNodeCheck(WtoA(key), msg, bAttr_R, etSSDLNodeCheck_Attr_R);
        }
        if (!m_vn->hasAttr(L"prefix"))
        {
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNodeԪ��ȱ��prefix����", formatedPath.c_str(), WtoA(key).c_str());
            SSDLNodeCheck(WtoA(key), msg, bAttr_O, etSSDLNodeCheck_Attr_O);
        }
        if (!m_vn->hasAttr(L"lnType"))
        {
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNodeԪ��ȱ��lnType����", formatedPath.c_str(), WtoA(key).c_str());
            SSDLNodeCheck(WtoA(key), msg, bAttr_O, etSSDLNodeCheck_Attr_O);
        }

        lnMap.insert(make_pair(key, ln));
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetSSDTerminalCache(string key, string &formatedPath, TerminalCacheMap &terminalMap)
{
    try
    {
        // �ж������Ƿ�ѡ��
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDTerminalCheck_Attr_R").bEnabled;
        ErrorType etSSDTerminalCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDTerminalCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDTerminalCheck_Attr_O").bEnabled;
        ErrorType etSSDTerminalCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDTerminalCheck_Attr_O").etValue;

        bool bParentChecked = CSSDTerminalCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 6); // 7�����ԣ�name��connectivityNode��substationName��voltageLevelName�� bayName��cnodeName��7������
        AddDueElementInfo(bParentChecked, bAttr_O, 1); // 7�����ԣ�desc��7������
        string terminalName = WtoA(VtdGetAttributeValue(m_vn, L"name"));
        string newKey = key + "/" + terminalName;
        
        TerminalCache terminal;
        terminal.key = AtoW(newKey);
        terminal.offset = GetOffsetOfVTD();
        if (m_vn->hasAttr(L"name"))
        {
            terminal.name = VtdGetAttributeValue(m_vn, L"name");
            SSDTerminalCheck_NameNormativeCheck(newKey, formatedPath, terminalName);
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]TerminalԪ��ȱ��name����", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_R, etSSDTerminalCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            terminal.desc = VtdGetAttributeValue(m_vn, L"desc");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]TerminalԪ��ȱ��desc����", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_O, etSSDTerminalCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"connectivityNode"))
        {
            terminal.connectivityNode = VtdGetAttributeValue(m_vn, L"connectivityNode");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]TerminalԪ��ȱ��connectivityNode����", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_R, etSSDTerminalCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"substationName"))
        {
            terminal.substationName = VtdGetAttributeValue(m_vn, L"substationName");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]TerminalԪ��ȱ��substationName����", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_R, etSSDTerminalCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"voltageLevelName"))
        {
            terminal.voltageLevelName = VtdGetAttributeValue(m_vn, L"voltageLevelName");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]TerminalԪ��ȱ��voltageLevelName����", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_R, etSSDTerminalCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"bayName"))
        {
            terminal.bayName = VtdGetAttributeValue(m_vn, L"bayName");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]TerminalԪ��ȱ��bayName����", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_R, etSSDTerminalCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"cNodeName"))
        {
            terminal.cnodeName = VtdGetAttributeValue(m_vn, L"cNodeName");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]TerminalԪ��ȱ��cNodeName����", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_R, etSSDTerminalCheck_Attr_R);
        }

        string path = FormatXMLMessage("%s, Terminal name=%s", formatedPath.c_str(), terminalName.c_str());
        // terminal.key = AtoW(key);
        m_AllTerminalMap.insert(make_pair(AtoW(newKey), terminal));
        if (terminalMap.find(terminal.name) == terminalMap.end())
        {
            terminalMap.insert(make_pair(terminal.name, terminal));
        }
        else
        {
            // ͬװ�����ظ�
            SSDTerminalCheck_UniqueCheck(newKey, path, terminal);
        }        
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool CCacheMgr::GetSSDCommonEquipmentCache(string key, string &formatedPath, CommonEquipmentCacheMap &ceMap)
{
    //TabChanger  SubEquipment 
    try
    {
        // �ж������Ƿ�ѡ��
        //bool bAttr = CConfigMgr::GetInstance()->GetSubItemConfig("SSDCommonEquipmentCheck_Attr").bEnabled;
        //bool bParentChecked = CSSDCommonEquipmentCheck::GetInstance()->IsEnabled();
        //AddDueElementInfo(bParentChecked, bAttr, 4); // 4�����ԣ�name��desc�� virtual��type

        //string comeqName = WtoA(VtdGetAttributeValue(m_vn, L"name"));
        //CommonEquipmentCache equipment;
        //string newKey = key + "/" + comeqName;
        //equipment.key = AtoW(newKey);
        //if (m_vn->hasAttr(L"name"))
        //{
        //    equipment.name = VtdGetAttributeValue(m_vn, L"name");
        //}
        //else
        //{
        //    string msg = FormatXMLMessage("[%s, Equipment name=%s]�豸ȱ��name����", formatedPath.c_str(), comeqName.c_str());
        //    SSDCommonEquipmentCheck(newKey, msg, bAttr);
        //}

        //if (m_vn->hasAttr(L"desc"))
        //{
        //    equipment.desc = VtdGetAttributeValue(m_vn, L"desc");
        //} 
        //else
        //{
        //    string msg = FormatXMLMessage("[%s, Equipment name=%s]�豸ȱ��desc����", formatedPath.c_str(), comeqName.c_str());
        //    SSDCommonEquipmentCheck(newKey, msg, bAttr);
        //}

        //if (m_vn->hasAttr(L"virtual"))
        //{
        //    equipment.vir = VtdGetAttributeValue(m_vn, L"virtual");
        //} 
        //else
        //{
        //    string msg = FormatXMLMessage("[%s, Equipment name=%s]�豸ȱ��virtual����", formatedPath.c_str(), comeqName.c_str());
        //    SSDCommonEquipmentCheck(newKey, msg, bAttr);
        //}

        //if (m_vn->hasAttr(L"type"))
        //{
        //    equipment.type = VtdGetAttributeValue(m_vn, L"type");
        //} 
        //else
        //{
        //    string msg = FormatXMLMessage("[%s, Equipment name=%s]�豸ȱ��type����", formatedPath.c_str(), comeqName.c_str());
        //    SSDCommonEquipmentCheck(newKey, msg, bAttr);
        //}
        //ceMap.insert(make_pair(equipment.name, equipment));
    }
    catch (...)
    {
    	return false;
    }
    return true;
}

bool CCacheMgr::GetSSDConnectivityNodeCache(string key, string &formatedPath, ConnectivityNodeCacheMap &connectivityNodeMap)
{
    try
    {
        // �ж������Ƿ�ѡ��
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConnectivityNodeCheck_Attr_R").bEnabled;
        ErrorType etSSDConnectivityNodeCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConnectivityNodeCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConnectivityNodeCheck_Attr_O").bEnabled;
        ErrorType etSSDConnectivityNodeCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConnectivityNodeCheck_Attr_O").etValue;

        bool bParentChecked = CSSDConnectivityNodeCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 2); // 3�����ԣ�ConnectivityNode��name��pathName�����Ƿ����;
        AddDueElementInfo(bParentChecked, bAttr_O, 1); // 3�����ԣ�ConnectivityNode��desc�����Ƿ����;
        
        string cnodeName = WtoA(VtdGetAttributeValue(m_vn, L"name"));
        ConnectivityNodeCache connectivityNode;
        string newKey = key + "/" + cnodeName;
        connectivityNode.key = AtoW(newKey);
        connectivityNode.pathMessage = AtoW(FormatXMLMessage("%s ConnectivityNode name=%s", formatedPath.c_str(), cnodeName.c_str()));
        connectivityNode.offset = GetOffsetOfVTD();
        connectivityNode.bUsed = false;
        if (m_vn->hasAttr(L"name"))
        {
            connectivityNode.name = VtdGetAttributeValue(m_vn, L"name");
            SSDConnectivityNodeCheck_NameNormativeCheck(newKey, WtoA(connectivityNode.pathMessage), WtoA(connectivityNode.name));
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, ConnectivityNode name=%s]ConnectivityNodeԪ��ȱ��name����", formatedPath.c_str(), cnodeName.c_str());
            SSDConnectivityNodeCheck(newKey, msg, bAttr_R, etSSDConnectivityNodeCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            connectivityNode.desc = VtdGetAttributeValue(m_vn, L"desc");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, ConnectivityNode name=%s]ConnectivityNodeԪ��ȱ��desc����", formatedPath.c_str(), cnodeName.c_str());
            SSDConnectivityNodeCheck(newKey, msg, bAttr_O, etSSDConnectivityNodeCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"pathName"))
        {
            connectivityNode.pathname = VtdGetAttributeValue(m_vn, L"pathName");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, ConnectivityNode name=%s]ConnectivityNodeԪ��ȱ��pathName����", formatedPath.c_str(), cnodeName.c_str());
            SSDConnectivityNodeCheck(newKey, msg, bAttr_R, etSSDConnectivityNodeCheck_Attr_R);
        }   

        // Ψһ�� ���
        if (connectivityNode.pathname.size() > 0)
        {
            if(m_AllcNodeMap.find(connectivityNode.pathname) != m_AllcNodeMap.end())
            {
                SSDConnectivityNodeCheck_PathUniqueCheck(WtoA(connectivityNode.key), WtoA(connectivityNode.pathMessage), connectivityNode);
            }
            else
            {
                m_AllcNodeMap.insert(make_pair(connectivityNode.pathname, connectivityNode));
            }
        }        
        
        connectivityNodeMap.insert(make_pair(connectivityNode.name, connectivityNode));
    }
    catch(...)
    {
        return  false;
    }
    return true;
}

bool CCacheMgr::GetSSDFunctionCache(string key, string &formatedPath, FunctionCacheMap &funcMap)
{
    //try
    //{
    //    FunctionCache func;
    //    if (m_vn->hasAttr(L"name"))
    //    {
    //        func.name = VtdGetAttributeValue(m_vn, L"name");
    //    } 
    //    else
    //    {
    //        //TODO
    //    }

    //    if (m_vn->hasAttr(L"desc"))
    //    {
    //        func.name = VtdGetAttributeValue(m_vn, L"desc");
    //    } 
    //    else
    //    {
    //        //TODO
    //    }

    //    if (m_vn->toElement(FIRST_CHILD))
    //    {
    //        do 
    //        {
    //            //TODO
    //        } while (m_vn->toElement(NEXT_SIBLING));
    //    }

    //    funcMap.insert(make_pair(func.name, func));
    //}
    //catch (...)
    //{
    //    return false;
    //}

    return true;
}

// ---------------------------------------------------------------------------------------------------------�����Լ��----------------------------------------------
void CCacheMgr::AddDueElementInfo(bool bBigItemChecked, bool bSubChecked, int attrCount)
{
    if (bBigItemChecked && bSubChecked)
    {
        m_DueSCDElementInfo += attrCount;
    }
}

bool CCacheMgr::SSDSubstationCheck(string key, string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSSDSubstationCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDSubstationCheck::GetInstance()->Report(key, etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SSDVoltageLevelCheck(string key, string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSSDVoltageLevelCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDVoltageLevelCheck::GetInstance()->Report(key, etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SSDBayCheck(string key, string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSSDBayCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDBayCheck::GetInstance()->Report(key, etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SSDPowerTransformerCheck(string key, string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSSDPowerTransformerCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDPowerTransformerCheck::GetInstance()->Report(key, etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SSDConductingEquipmentCheck(string key, string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSSDConductingEquipmentCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDConductingEquipmentCheck::GetInstance()->Report(key, etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SSDTerminalCheck(string key, string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSSDTerminalCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDTerminalCheck::GetInstance()->Report(key, etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SSDLNodeCheck(string key, string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSSDLNodeCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDLNodeCheck::GetInstance()->Report(key, etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SSDConnectivityNodeCheck(string key, string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSSDConnectivityNodeCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDConnectivityNodeCheck::GetInstance()->Report(key, etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SSDCommonEquipmentCheck(string key, string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSSDCommonEquipmentCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDCommonEquipmentCheck::GetInstance()->Report(key, etValue, row, xml, message.c_str());
        }
    }
    return true;
}
// ---------------------------------Header And Communication-----------------------------------------------------
bool CCacheMgr::SCDHeaderCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDHeaderCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDHeaderCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SCDCommunicationCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDCommunicationCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDCommunicationCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SCDSubNetWorkCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDSubNetWorkCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDSubNetWorkCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SCDConnectedAPCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDConnectedAPCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDConnectedAPCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SCDIPAddressCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDIPAddressCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDIPAddressCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SCDGSECheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDGSECheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDGSECheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SCDSMVCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDSMVCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDSMVCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SCDPhysConnCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDPhysConnCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDPhysConnCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}

// -----------------------------------------------------------------------------------DTT-----------------------------------------------------
bool CCacheMgr::SCDLNodeTypeCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDLNodeTypeCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDLNodeTypeCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SCDDOTypeCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDDOTypeCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDDOTypeCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SCDDATypeCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDDATypeCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDDATypeCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SCDEnumTypeCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDEnumTypeCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDEnumTypeCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}
// -----------------------------------------------------------------------------------IED------------------------------------------------------
bool CCacheMgr::SCDIEDCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDIEDCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDIEDCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}
bool CCacheMgr::SCDIEDServicesCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDIEDServicesCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDIEDServicesCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}
bool CCacheMgr::SCDIEDAccessPointCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDIEDAccessPointCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDIEDAccessPointCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}
bool CCacheMgr::SCDIEDLNCheck(string message, bool bSubItemChecked, ErrorType etValue)
{
    if (CSCDIEDLNCheck::GetInstance()->IsEnabled())
    {
        if (bSubItemChecked)
        {
            m_LoseSCDElementInfo += 1;
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSCDIEDLNCheck::GetInstance()->Report(etValue, row, xml, message.c_str());
        }
    }
    return true;
}




// -----------------------------------------------------------------------------------SSD�淶�Լ��------------------------------------------------------

bool CCacheMgr::SSDSubstationCheck_NormativeCheck(string key, string subStationName, string dType)
{
    if (CSSDSubstationCheck_Normative::GetInstance()->IsEnabled())
    {
        if (!CConfigMgr::GetInstance()->IsInSubstationTypeMap(dType))
        {
            string msg = FormatXMLMessage("[Substation name=%s]Substation���վ����Ϊ%s,�����ϱ�׼�е���һ����", subStationName.c_str(), dType.c_str());
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDSubstationCheck_Normative::GetInstance()->Report(key, CSSDSubstationCheck_Normative::GetInstance()->GetErrorType(), row, xml, msg.c_str());
        }      
    }
    return true;
}

bool CCacheMgr::SSDSubstationCheck_OtherCheck(string key, string subStationName, string tagName)
{
    if (CSSDSubstationCheck_Other::GetInstance()->IsEnabled())
    {
        string msg = FormatXMLMessage("[Substation name=%s]Substation����δ�涨�ڵ�%s", subStationName.c_str(), tagName.c_str());
        int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
        string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
        CSSDSubstationCheck_Other::GetInstance()->Report(key, CSSDSubstationCheck_Other::GetInstance()->GetErrorType(), row, xml, msg.c_str());      
    }
    return true;
}

bool CCacheMgr::SSDVoltageLevelCheck_UnNULLCheck(string key, string message)
{
    if (CSSDVoltageLevelCheck_UnNULL::GetInstance()->IsEnabled())
    {
        int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
        string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
        CSSDVoltageLevelCheck_UnNULL::GetInstance()->Report(key, CSSDVoltageLevelCheck_UnNULL::GetInstance()->GetErrorType(), row, xml, message.c_str());      
    }
    return true;
}

bool CCacheMgr::SSDVoltageLevelCheck_NormativeCheck(string key, string subStationName, string vtgLevel, bool bName)
{
    if (CSSDVoltageLevelCheck_Normative::GetInstance()->IsEnabled())
    {
        if (0 == CConfigMgr::GetInstance()->IsInVoltageLevelTypeMap(vtgLevel))
        {
            string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]��ѹ�ȼ�name�����ϱ�׼�е���һ����", subStationName.c_str(), vtgLevel.c_str());
            if (!bName)
            {
                msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]��ѹ�ȼ�desc�����ϱ�׼�е���һ����", subStationName.c_str(), vtgLevel.c_str());
            }            
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDVoltageLevelCheck_Normative::GetInstance()->Report(key,CSSDVoltageLevelCheck_Normative::GetInstance()->GetErrorType(), row, xml, msg.c_str());
        }   
    }
    return true;
}

bool CCacheMgr::SSDVoltageLevelCheck_UniqueCheck(string key, string message)
{
    if (CSSDVoltageLevelCheck_Unique::GetInstance()->IsEnabled())
    {
        int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
        string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
        CSSDVoltageLevelCheck_Unique::GetInstance()->Report(key, CSSDVoltageLevelCheck_Unique::GetInstance()->GetErrorType(), row, xml, message.c_str());
    }
    return true;
}

bool CCacheMgr::SSDVoltageLevelCheck_OtherCheck(string key, string subStationName, string vtgLevel, string tagName)
{
    if (CSSDVoltageLevelCheck_Other::GetInstance()->IsEnabled())
    {
        string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevel����δ�涨�ڵ�%s", subStationName.c_str(), vtgLevel.c_str(), tagName.c_str());
        int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
        string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
        CSSDVoltageLevelCheck_Other::GetInstance()->Report(key, CSSDVoltageLevelCheck_Other::GetInstance()->GetErrorType(), row, xml, msg.c_str());     
    }
    return true;
}

bool CCacheMgr::SSDVoltageLevelCheck_SortCheck(string key, string subStationName, int offset)
{
    if (CSSDVoltageLevelCheck_Sort::GetInstance()->IsEnabled())
    {
        string msg = FormatXMLMessage("[Substation name=%s]VoltageLevel��ѹ�ȼ�δ���մӸߵ��͵�˳���������", subStationName.c_str());
        int row = pChecker->CalcRowNumberFromOffset(offset);
        string xml = pChecker->GetRowContentByOffset(offset);
        CSSDVoltageLevelCheck_Sort::GetInstance()->Report(key, CSSDVoltageLevelCheck_Sort::GetInstance()->GetErrorType(), row, xml, msg.c_str());  
    }
    return true;
}

// һ���豸���BayԪ�ص�name��desc�����Ƿ���Ϲ淶
bool CCacheMgr::SSDBayCheck_AttrNormativeCheck(string key, string path, BayCache &bay)
{
    if (CSSDBayCheck_AttrNormative::GetInstance()->IsEnabled())
    {
        // ��ȡһ�����豸
        wstring eqName, eqDesc, eqType;
        if (bay.ptMap.size() > 0)
        {
           eqName = bay.ptMap.begin()->second.name;
           eqDesc = bay.ptMap.begin()->second.desc;
           eqType = bay.ptMap.begin()->second.type;
        }
        else if (bay.ceMap.size() > 0)
        {
            ConductingEquipmentCacheMap::iterator it = bay.ceMap.begin();
            while (it != bay.ceMap.end())
            {
                ConductingEquipmentCache *ce = &it->second;
                if ((ce->type.find(L"EBUS") != wstring::npos) ||
                    (ce->type.find(L"LIN") != wstring::npos) ||
                    (ce->type.find(L"IFL") != wstring::npos) ||
                    (ce->type.find(L"CAP") != wstring::npos) ||
                    (ce->type.find(L"REA") != wstring::npos))
                {
                    eqName = ce->name;
                    eqDesc = ce->desc;
                    eqType = ce->type;
                    break;
                }
                else if (ce->type.find(L"CBR") != wstring::npos)
                {
                    eqName = ce->name;
                    eqDesc = ce->desc;
                    eqType = ce->type;
                }
                it++;
            }
            // ��Ϊ�ҵ� ��ѡ��װ��
            if (eqName.size() == 0)
            {
                eqName = bay.ceMap.begin()->second.name;
                eqDesc = bay.ceMap.begin()->second.desc;
                eqType = bay.ceMap.begin()->second.type;
            }
        }

        // �жϼ������
        if (eqName != bay.name && eqName.size() > 0)
        {
            string msg = FormatXMLMessage("[%s]�����name����������һ�����豸��name, ����һ�����豸Ϊ%s", path.c_str(), WtoA(eqName).c_str());
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDBayCheck_AttrNormative::GetInstance()->Report(key, CSSDBayCheck_AttrNormative::GetInstance()->GetErrorType(), row, xml, msg.c_str());
        }
        // �жϼ������
        if ((eqDesc +  + L"���") != bay.desc && eqDesc.size() > 0)
        {
            string msg = FormatXMLMessage("[%s]�����desc����������һ����Ԫ���� ��������+��� �ķ�ʽ����,����һ���豸����Ϊ%s", path.c_str(), WtoA(eqDesc).c_str());
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDBayCheck_AttrNormative::GetInstance()->Report(key, CSSDBayCheck_AttrNormative::GetInstance()->GetErrorType(), row, xml, msg.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SSDBayCheck_dTypeNormativeCheck(string key, string vtgLevel, string bayName, string dType)
{
    if (CSSDBayCheck_dTypeNormative::GetInstance()->IsEnabled())
    {
        if (!CConfigMgr::GetInstance()->IsInBayTypeMap(dType))
        {
            string msg = FormatXMLMessage("[VoltageLevel name=%s Bay name=%s]�����dTypeΪ%s,�����Ϲ淶", vtgLevel.c_str(), bayName.c_str(), dType.c_str());
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDBayCheck_dTypeNormative::GetInstance()->Report(key, CSSDBayCheck_dTypeNormative::GetInstance()->GetErrorType(), row, xml, msg.c_str());
        }
    }
    return true;
}

// 
bool CCacheMgr::SSDPowerTransformerCheck_UniqueCheck(string key, string path)
{
    if (CSSDPowerTransformerCheck_Unique::GetInstance()->IsEnabled())
    {
        string msg = FormatXMLMessage("[%s]����ں��ж����ѹ���豸��Ӧֻ����һ����ѹ��", path.c_str());
        int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
        string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
        CSSDPowerTransformerCheck_Unique::GetInstance()->Report(key, CSSDPowerTransformerCheck_Unique::GetInstance()->GetErrorType(), row, xml, msg.c_str());
    }

    return true;
}

bool CCacheMgr::SSDPowerTransformerCheck_AttrNormativeCheck(string key, string path, string ptName)
{
    if (CSSDPowerTransformerCheck_AttrNormative::GetInstance()->IsEnabled())
    {
        if (ptName.find("PTR") == string::npos)
        {
            string msg = FormatXMLMessage("[%s]��ѹ���豸name=%sӦ���á����ʹ��롱+�����ȱ�š�����", path.c_str(), ptName.c_str());
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDPowerTransformerCheck_AttrNormative::GetInstance()->Report(key, CSSDPowerTransformerCheck_AttrNormative::GetInstance()->GetErrorType(), row, xml, msg.c_str());
        }        
    }
    return true;
}

bool CCacheMgr::SSDPowerTransformerCheck_WindingNormativeCheck(string key, string path)
{
    if (CSSDPowerTransformerCheck_WindingNormative::GetInstance()->IsEnabled())
    {
        string msg = FormatXMLMessage("[%s]��ѹ���豸���ٰ���������ѹ��������Ԫ��", path.c_str());
        int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
        string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
        CSSDPowerTransformerCheck_WindingNormative::GetInstance()->Report(key, CSSDPowerTransformerCheck_WindingNormative::GetInstance()->GetErrorType(), row, xml, msg.c_str());
    }
    return true;
}

bool CCacheMgr::SSDTerminalCheck_NameNormativeCheck(string key, string path, string tmlName)
{
    if (CSSDTerminalCheck_NameNormative::GetInstance()->IsEnabled())
    {
        if (!((tmlName.size() > 0) &&
            (tmlName.at(0) == 'T' || tmlName.at(0) == 't') &&
            (tmlName.at(1) >= '0' && tmlName.at(1) <= '9')))
        {
            string msg = FormatXMLMessage("[%s]Terminal����name=%s����δ����Tn��������", path.c_str(), tmlName.c_str());
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDTerminalCheck_NameNormative::GetInstance()->Report(key, CSSDTerminalCheck_NameNormative::GetInstance()->GetErrorType(), row, xml, msg.c_str());
        }
    }
    return true;
}


bool CCacheMgr::SSDTerminalCheck_UniqueCheck(string key, string path, TerminalCache &terminal)
{
    if (CSSDTerminalCheck_Unique::GetInstance()->IsEnabled())
    {
        string msg = FormatXMLMessage("[%s]Terminal������ͬһװ���ڲ�Ψһ", path.c_str());
        int row = pChecker->CalcRowNumberFromOffset(terminal.offset);
        string xml = pChecker->GetRowContentByOffset(terminal.offset);
        CSSDTerminalCheck_Unique::GetInstance()->Report(key, CSSDTerminalCheck_Unique::GetInstance()->GetErrorType(), row, xml, msg.c_str());
    }
    return true;
}

bool CCacheMgr::SSDTerminalCheck_ValidCheck()
{
    if (CSSDTerminalCheck_Valid::GetInstance()->IsEnabled())
    {
        std::map<std::wstring, TerminalCache>::iterator it = m_AllTerminalMap.begin();
        while(it != m_AllTerminalMap.end())
        {
            TerminalCache *tml = &it->second;
            // ������Ч�Լ��
            wstring cnode = tml->substationName + L"/" + tml->voltageLevelName + L"/" + tml->bayName + L"/" + tml->cnodeName;
            if (cnode != tml->connectivityNode)
            {
                string msg = FormatXMLMessage("[%s]Terminal������connectivityNode��substationName��voltageLevelName��bayName��cNodeName��ָ����[%s]��һ��", WtoA(tml->key).c_str(), WtoA(cnode).c_str());
                int row = pChecker->CalcRowNumberFromOffset(tml->offset);
                string xml = pChecker->GetRowContentByOffset(tml->offset);
                CSSDTerminalCheck_Valid::GetInstance()->Report(WtoA(tml->key), CSSDTerminalCheck_Valid::GetInstance()->GetErrorType(), row, xml, msg.c_str());
            }
            else
            {
                // һ�������,��CNode���бȽ�
                std::map<std::wstring, ConnectivityNodeCache>::iterator itCNode = m_AllcNodeMap.find(tml->connectivityNode);
                if (itCNode == m_AllcNodeMap.end())
                {
                    string msg = FormatXMLMessage("[%s]Terminal������connectivityNode��ָ���󲻴���", WtoA(tml->key).c_str());
                    int row = pChecker->CalcRowNumberFromOffset(tml->offset);
                    string xml = pChecker->GetRowContentByOffset(tml->offset);
                    CSSDTerminalCheck_Valid::GetInstance()->Report(WtoA(tml->key), CSSDTerminalCheck_Valid::GetInstance()->GetErrorType(), row, xml, msg.c_str());
                }
            }          
            it++;
        }
    }
    return true;
}

bool CCacheMgr::SSDConnectivityNodeCheck_PathUniqueCheck(string key, string path, ConnectivityNodeCache& cNode)
{
    if (CSSDConnectivityNodeCheck_PathUnique::GetInstance()->IsEnabled())
    {
        string msg = FormatXMLMessage("[%s]ConnectivityNode��pathName�����ظ�", path.c_str());
        int row = pChecker->CalcRowNumberFromOffset(cNode.offset);
        string xml = pChecker->GetRowContentByOffset(cNode.offset);
        CSSDConnectivityNodeCheck_PathUnique::GetInstance()->Report(key, CSSDConnectivityNodeCheck_PathUnique::GetInstance()->GetErrorType(), row, xml, msg.c_str());
    }
    return true;
}

bool CCacheMgr::SSDConnectivityNodeCheck_UnUsedCheck()
{
    if (CSSDConnectivityNodeCheck_UnUsed::GetInstance()->IsEnabled())
    {
        std::map<std::wstring, TerminalCache>::iterator it = m_AllTerminalMap.begin();
        while(it != m_AllTerminalMap.end())
        {
            TerminalCache *tml = &it->second;
            std::map<std::wstring, ConnectivityNodeCache>::iterator itCNode = m_AllcNodeMap.find(tml->connectivityNode);
            if (itCNode != m_AllcNodeMap.end())
            {
                itCNode->second.bUsed = true;
            }
            it++;
        }

        std::map<std::wstring, ConnectivityNodeCache>::iterator itCNode = m_AllcNodeMap.begin();
        while (itCNode != m_AllcNodeMap.end())
        {
            if (!itCNode->second.bUsed)
            {
                string msg = FormatXMLMessage("[%s]ConnectivityNodeδ��Terminal����", WtoA(itCNode->second.key).c_str());
                int row = pChecker->CalcRowNumberFromOffset(itCNode->second.offset);
                string xml = pChecker->GetRowContentByOffset(itCNode->second.offset);
                CSSDConnectivityNodeCheck_UnUsed::GetInstance()->Report(WtoA(itCNode->second.key), CSSDConnectivityNodeCheck_UnUsed::GetInstance()->GetErrorType(), row, xml, msg.c_str());
            }
            itCNode++;
        }
    }
    return true;
}

bool CCacheMgr::SSDConnectivityNodeCheck_NameNormativeCheck(string key, string path, string cNodeName)
{
    if (CSSDConnectivityNodeCheck_NameNormative::GetInstance()->IsEnabled())
    {
        if (!((cNodeName.size() > 0) &&
            (cNodeName.at(0) == 'C' || cNodeName.at(0) == 'c') &&
            (cNodeName.at(1) >= '0' && cNodeName.at(1) <= '9')))
        {
            string msg = FormatXMLMessage("[%s]ConnectivityNode����name=%s����δ����Cn��������", path.c_str(), cNodeName.c_str());
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDConnectivityNodeCheck_NameNormative::GetInstance()->Report(key, CSSDConnectivityNodeCheck_NameNormative::GetInstance()->GetErrorType(), row, xml, msg.c_str());
        }
    }
    return true;
}

bool CCacheMgr::SSDConductingEquipmentCheck_UniqueCheck(string key, string path)
{
    if (CSSDConductingEquipmentCheck_Unique::GetInstance()->IsEnabled())
    {
        string msg = FormatXMLMessage("[%s]ConductingEquipment��ͬһ����ڴ����ظ�", path.c_str());
        int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
        string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
        CSSDConductingEquipmentCheck_Unique::GetInstance()->Report(key, CSSDConductingEquipmentCheck_Unique::GetInstance()->GetErrorType(), row, xml, msg.c_str());
    }
    return true;
}

bool CCacheMgr::SSDConductingEquipmentCheck_IFLEquipmentCheck(string key, ConductingEquipmentCache &eq)
{
    if (CSSDConductingEquipmentCheck_IFLEquipment::GetInstance()->IsEnabled())
    {
        wstring first = eq.name.substr(0, 2);
        wstring last = eq.name.substr(eq.name.size()-1, 1);
        if (!((first == L"LN") &&
            ((last == L"L") || 
            (last == L"X"))))
        {
            string msg = FormatXMLMessage("[%s]�������豸ǰ׺ӦΪLN����׺ӦΪL/X, ��ǰ�豸����Ϊ%s, ǰ׺Ϊ%s, ��׺Ϊ%s", WtoA(eq.path).c_str(), WtoA(eq.name).c_str(), WtoA(first).c_str(), WtoA(last).c_str());
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDConductingEquipmentCheck_IFLEquipment::GetInstance()->Report(key, CSSDConductingEquipmentCheck_IFLEquipment::GetInstance()->GetErrorType(), row, xml, msg.c_str());
        }
    }
    return true;
}


