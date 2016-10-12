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
// -----------------------------------------SSD完整性------------------------------------------
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
    // 检测器
    pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
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
                // 判断子项是否被选中
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Attr_R").bEnabled;
                ErrorType etAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Attr_R").etValue;
                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Attr_O").bEnabled;
                ErrorType etAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Attr_O").etValue;
                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Child").bEnabled;      
                ErrorType etChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDCheck_Child").etValue;
                bool bParentChecked = CSCDIEDCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 1); // IED元素含有name等1个属性
                AddDueElementInfo(bParentChecked, bAttr_O, 4); // IED元素含有desc, type, manufacturer 和configVersion等4个属性
                AddDueElementInfo(bParentChecked, bChild, 1); // IED元素含有2个子元素Servises、AccessPoint。其中AccessPoint元素为必选

                string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                if (!m_vn->hasAttr(L"name"))
                {
                    string msg = FormatXMLMessage("[IED name=%s]IED元素下缺少name属性", name.c_str());
                    SCDIEDCheck(msg, bAttr_R, etAttr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s]IED元素下缺少desc属性", name.c_str());
                    SCDIEDCheck(msg, bAttr_O, etAttr_O);
                }
                if (!m_vn->hasAttr(L"type"))
                {
                    string msg = FormatXMLMessage("[IED name=%s]IED元素下缺少type属性", name.c_str());
                    SCDIEDCheck(msg, bAttr_O, etAttr_O);
                }
                if (!m_vn->hasAttr(L"manufacturer"))
                {
                    string msg = FormatXMLMessage("[IED name=%s]IED元素下缺少manufacturer属性", name.c_str());
                    SCDIEDCheck(msg, bAttr_O, etAttr_O);
                }
                if (!m_vn->hasAttr(L"configVersion"))
                {
                    string msg = FormatXMLMessage("[IED name=%s]IED元素下缺少configVersion属性", name.c_str());
                    SCDIEDCheck(msg, bAttr_O, etAttr_O);
                }

                setIntAddr.clear(); // 清空IED下的intAddr set

                IEDCache iedCache;
                iedCache.name = VtdGetAttributeValue(m_vn, L"name");
                CreatInterval(iedCache.name);

                iedCache.offset = GetOffsetOfVTD();
                GetIEDCRC(iedCache);
                GetServisesCache(iedCache);
                GetLDCache(iedCache);

                // 将IED放入Map中
                IEDMap::iterator it = m_SCDCache.mapIED.find(iedCache.name);
                if (it != m_SCDCache.mapIED.end())
                {
                    int row = pChecker->CalcRowNumberFromOffset(iedCache.offset);
                    string xml = pChecker->GetRowContentByOffset(iedCache.offset);
                    string msg = "IED名称不唯一:IED name=%s";
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

// 取得Services信息
bool CCacheMgr::GetServisesCache(IEDCache &iedCache)
{
    try
    {
        if (m_vn->toElement(FIRST_CHILD, L"Services"))
        {
            do 
            {
                // 判断子项是否被选中
                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_Child_O").bEnabled;
                ErrorType etChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_Child_O").etValue;
                bool bParentChecked = CSCDIEDServicesCheck::GetInstance()->IsEnabled();
                // Servises元素应含有SettingGroups, ConfDataSet, DynDataSet,ConfReportControl,ConfLogControl,ReportSettings, 
                // LogSettings, GSESettings,SMVSettings, GOOSE, GSSE, 及ConfLNs等12个子元素。
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
                            // SettingGroups元素含有SGEdit和ConfSG等2个子元素
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
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素SettingGroups缺少SGEdit子元素", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SettingGroups_Child, etSCDIEDServicesCheck_SettingGroups_Child);
                            }
                            if (!bHaveConfSG)
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素SettingGroups缺少ConfSG子元素", WtoA(iedCache.name).c_str());
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
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ConfDataSet_Attr_R, 1);//  ConfDataSet元素含有max 1个属性
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ConfDataSet_Attr_O, 2);//  ConfDataSet元素含有maxAttributes和modify等2个属性

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ConfDataSet元素缺少max属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfDataSet_Attr_R, etSCDIEDServicesCheck_ConfDataSet_Attr_R);
                            }
                            if (!m_vn->hasAttr(L"maxAttributes"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ConfDataSet元素缺少maxAttributes属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfDataSet_Attr_O, etSCDIEDServicesCheck_ConfDataSet_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"modify"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ConfDataSet元素缺少modify属性", WtoA(iedCache.name).c_str());
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
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_DynDataSet_Attr_R, 1);//  DynDataSet元素含有max等1个属性
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_DynDataSet_Attr_O, 1);//  DynDataSet元素含有maxAttributes等1个属性

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素DynDataSet元素缺少max属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_DynDataSet_Attr_R, etSCDIEDServicesCheck_DynDataSet_Attr_R);
                            }
                            if (!m_vn->hasAttr(L"maxAttributes"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素DynDataSet元素缺少maxAttributes属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_DynDataSet_Attr_O, etSCDIEDServicesCheck_DynDataSet_Attr_O);
                            }

                        }
                        else if (tag == L"ConfReportControl")
                        {
                            bHaveConfReportControl = true;
                            bool bSCDIEDServicesCheck_ConfReportControl_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfReportControl_Attr").bEnabled;
                            ErrorType etSCDIEDServicesCheck_ConfReportControl_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfReportControl_Attr").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ConfReportControl_Attr, 1);// ConfReportControl元素含有max属性

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ConfReportControl元素缺少max属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfReportControl_Attr, etSCDIEDServicesCheck_ConfReportControl_Attr);
                            }
                        }
                        else if (tag == L"ConfLogControl")
                        {
                            bHaveConfLogControl = true;
                            bool bSCDIEDServicesCheck_ConfLogControl_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfLogControl_Attr").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_ConfLogControl_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfLogControl_Attr").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ConfLogControl_Attr, 1);// ConfLogControl元素含有max属性

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ConfLogControl元素缺少max属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfLogControl_Attr, etSCDIEDServicesCheck_ConfLogControl_Attr);
                            }
                        }
                        else if (tag == L"ReportSettings")
                        {
                            bHaveReportSettings = true;
                            bool bSCDIEDServicesCheck_ReportSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ReportSettings_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_ReportSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ReportSettings_Attr_O").etValue;
                            // ReportSettings元素含有cbName, dataset, rptID, optFields, bufTime, trgOps和intgPd等7个属性。
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ReportSettings_Attr_O, 7);

                            if (!m_vn->hasAttr(L"cbName"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ReportSettings元素缺少cbName属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"datSet"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ReportSettings元素缺少datSet属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"rptID"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ReportSettings元素缺少rptID属性", WtoA(iedCache.name).c_str());
                                    "ReportSettings元素缺少rptID属性";
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"optFields"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ReportSettings元素缺少optFields属性", WtoA(iedCache.name).c_str());
                                    "ReportSettings元素缺少optFields属性";
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"bufTime"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ReportSettings元素缺少bufTime属性", WtoA(iedCache.name).c_str());
                                    "ReportSettings元素缺少bufTime属性";
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"trgOps"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ReportSettings元素缺少trgOps属性", WtoA(iedCache.name).c_str());
                                    "ReportSettings元素缺少trgOps属性";
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"intgPd"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ReportSettings元素缺少intgPd属性", WtoA(iedCache.name).c_str());
                                    "ReportSettings元素缺少intgPd属性";
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ReportSettings_Attr_O, etSCDIEDServicesCheck_ReportSettings_Attr_O);
                            }
                        }
                        else if (tag == L"LogSettings")
                        {
                            bHaveLogSettings = true;
                            bool bSCDIEDServicesCheck_LogSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_LogSettings_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_LogSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_LogSettings_Attr_O").etValue;
                            // LogSettings元素含有cbName, dataset, logEna, trgOps和intgPd等5个属性
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_LogSettings_Attr_O, 5);

                            if (!m_vn->hasAttr(L"cbName"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素LogSettings元素缺少cbName属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_LogSettings_Attr_O, etSCDIEDServicesCheck_LogSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"datSet"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素LogSettings元素缺少datSet属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_LogSettings_Attr_O, etSCDIEDServicesCheck_LogSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"logEna"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素LogSettings元素缺少logEna属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_LogSettings_Attr_O, etSCDIEDServicesCheck_LogSettings_Attr_O);
                            }                            
                            if (!m_vn->hasAttr(L"trgOps"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素LogSettings元素缺少trgOps属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_LogSettings_Attr_O, etSCDIEDServicesCheck_LogSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"intgPd"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素LogSettings元素缺少intgPd属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_LogSettings_Attr_O, etSCDIEDServicesCheck_LogSettings_Attr_O);
                            }
                        }
                        else if (tag == L"GSESettings")
                        {
                            bHaveGSESettings = true;
                            bool bSCDIEDServicesCheck_GSESettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GSESettings_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_GSESettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GSESettings_Attr_O").etValue;
                            // GSESettings元素含有cbName, dataset, appID和dataLabel等4个属性
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_GSESettings_Attr_O, 4);

                            if (!m_vn->hasAttr(L"cbName"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素GSESettings元素缺少cbName属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GSESettings_Attr_O, etSCDIEDServicesCheck_GSESettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"datSet"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素GSESettings元素缺少datSet属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GSESettings_Attr_O, etSCDIEDServicesCheck_GSESettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"appID"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素GSESettings元素缺少appID属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GSESettings_Attr_O, etSCDIEDServicesCheck_GSESettings_Attr_O);
                            }                            
                            if (!m_vn->hasAttr(L"dataLabel"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素GSESettings元素缺少dataLabel属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GSESettings_Attr_O, etSCDIEDServicesCheck_GSESettings_Attr_O);
                            }
                        }
                        else if (tag == L"SMVSettings")
                        {
                            bHaveSMVSettings = true;
                            bool bSCDIEDServicesCheck_SMVSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_SMVSettings_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_SMVSettings_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_SMVSettings_Attr_O").etValue;
                            // SMVSettings元素含有cbName, dataset, svILD, optFields和 smpRate等5个属性
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_SMVSettings_Attr_O, 5);

                            if (!m_vn->hasAttr(L"cbName"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素SMVSettings元素缺少cbName属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SMVSettings_Attr_O, etSCDIEDServicesCheck_SMVSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"datSet"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素SMVSettings元素缺少datSet属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SMVSettings_Attr_O, etSCDIEDServicesCheck_SMVSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"svID"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素SMVSettings元素缺少svID属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SMVSettings_Attr_O, etSCDIEDServicesCheck_SMVSettings_Attr_O);
                            }                            
                            if (!m_vn->hasAttr(L"optFields"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素SMVSettings元素缺少optFields属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SMVSettings_Attr_O, etSCDIEDServicesCheck_SMVSettings_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"smpRate"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素SMVSettings元素缺少smpRate属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_SMVSettings_Attr_O, etSCDIEDServicesCheck_SMVSettings_Attr_O);
                            }
                        }
                        else if (tag == L"GOOSE")
                        {
                            bHaveGOOSE = true;
                            bool bSCDIEDServicesCheck_GOOSE_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GOOSE_Attr").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_GOOSE_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GOOSE_Attr").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_GOOSE_Attr, 1);// GOOSE元素含有max属性

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素GOOSE元素缺少max属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GOOSE_Attr, etSCDIEDServicesCheck_GOOSE_Attr);
                            }
                        }
                        else if (tag == L"GSSE")
                        {
                            bHaveGSSE = true;
                            bool bSCDIEDServicesCheck_GSSE_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GSSE_Attr").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_GSSE_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_GSSE_Attr").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_GSSE_Attr, 1);// GSSE元素含有max属性

                            if (!m_vn->hasAttr(L"max"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素GSSE元素缺少max属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_GSSE_Attr, etSCDIEDServicesCheck_GSSE_Attr);
                            }
                        }
                        else if (tag == L"ConfLNs")
                        {
                            bHaveConfLNs = true;
                            bool bSCDIEDServicesCheck_ConfLNs_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfLNs_Attr_O").bEnabled; 
                            ErrorType etSCDIEDServicesCheck_ConfLNs_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDServicesCheck_ConfLNs_Attr_O").etValue;
                            AddDueElementInfo(bParentChecked, bSCDIEDServicesCheck_ConfLNs_Attr_O, 2);// ConfLNs元素含有fixPrefix和fixLnInst属性

                            if (!m_vn->hasAttr(L"fixPrefix"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ConfLNs元素缺少fixPrefix属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfLNs_Attr_O, etSCDIEDServicesCheck_ConfLNs_Attr_O);
                            }
                            if (!m_vn->hasAttr(L"fixLnInst"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s]IED的Services子元素ConfLNs元素缺少fixLnInst属性", WtoA(iedCache.name).c_str());
                                SCDIEDServicesCheck(msg, bSCDIEDServicesCheck_ConfLNs_Attr_O, etSCDIEDServicesCheck_ConfLNs_Attr_O);
                            }
                        }

                    } while (m_vn->toElement(NEXT_SIBLING));

                    m_vn->toElement(PARENT);
                }

                if (!bHaveSettingGroups)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少SettingGroups子元素", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveConfDataSet)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少ConfDataSet子元素", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveDynDataSet)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少DynDataSet子元素", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveConfReportControl)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少ConfReportControl子元素", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveConfLogControl)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少ConfLogControl子元素", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveReportSettings)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少ReportSettings子元素", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveLogSettings)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少LogSettings子元素", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveGSESettings)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少GSESettings子元素", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveSMVSettings)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少SMVSettings子元素", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveGOOSE)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少GOOSE子元素", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveGSSE)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少GSSE子元素", WtoA(iedCache.name).c_str());
                    SCDIEDServicesCheck(msg, bChild, etChild);
                }
                if (!bHaveConfLNs)
                {
                    string msg = FormatXMLMessage("[IED name=%s]Services元素缺少ConfLNs子元素", WtoA(iedCache.name).c_str());
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
                // 判断子项是否被选中
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Attr_R").bEnabled;
                ErrorType etSCDIEDAccessPointCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Attr_R").etValue;
                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Attr_O").bEnabled;
                ErrorType etSCDIEDAccessPointCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Attr_O").etValue;
                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Child").bEnabled;
                ErrorType etSCDIEDAccessPointCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Child").etValue;
                bool bParentChecked = CSCDIEDAccessPointCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 1); // AccessPoint元素含有name等4个属性
                AddDueElementInfo(bParentChecked, bAttr_O, 3); // AccessPoint元素含有desc, router和clock等4个属性
                AddDueElementInfo(bParentChecked, bChild, 1); // AccessPoint元素含有Server子元素

                string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                if (!m_vn->hasAttr(L"name"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]AccessPoint元素缺少name属性", WtoA(iedCache.name).c_str(), name.c_str());
                    SCDIEDAccessPointCheck(msg, bAttr_R, etSCDIEDAccessPointCheck_Attr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]AccessPoint元素缺少desc属性", WtoA(iedCache.name).c_str(), name.c_str());
                    SCDIEDAccessPointCheck(msg, bAttr_O, etSCDIEDAccessPointCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"router"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]AccessPoint元素缺少router属性", WtoA(iedCache.name).c_str(), name.c_str());
                    SCDIEDAccessPointCheck(msg, bAttr_O, etSCDIEDAccessPointCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"clock"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]AccessPoint元素缺少clock属性", WtoA(iedCache.name).c_str(), name.c_str());
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
                    AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_Server_Attr, 2); // Server元素含有timeout和desc等2个属性
                    AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_Server_Child, 2); // Server元素含有子元素Authentication认证元素、LDevice逻辑设备元素、Association关联元素。其中Authentication、LDevice为必须元素

                    if (!m_vn->hasAttr(L"timeout"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server元素缺少timeout属性", WtoA(iedCache.name).c_str(), name.c_str());
                        SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Server_Attr, etSCDIEDAccessPointCheck_Server_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server元素缺少desc属性", WtoA(iedCache.name).c_str(), name.c_str());
                        SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Server_Attr, etSCDIEDAccessPointCheck_Server_Attr_O);
                    }

                    if (m_vn->toElement(FIRST_CHILD, L"Authentication"))
                    {
                        bool bSCDIEDAccessPointCheck_Authentication_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Authentication_Attr_O").bEnabled;
                        ErrorType etSCDIEDAccessPointCheck_Authentication_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Authentication_Attr_O").etValue;
                        // Authentication元素含有none, password, weak, strong 和 certificate等5个属性
                        AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_Authentication_Attr_O, 5); 
                        if (!m_vn->hasAttr(L"none"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Authentication缺少none属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Authentication_Attr_O, etSCDIEDAccessPointCheck_Authentication_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"password"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Authentication缺少password属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Authentication_Attr_O, etSCDIEDAccessPointCheck_Authentication_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"weak"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Authentication缺少weak属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Authentication_Attr_O, etSCDIEDAccessPointCheck_Authentication_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"strong"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Authentication缺少strong属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Authentication_Attr_O, etSCDIEDAccessPointCheck_Authentication_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"certificate"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Authentication缺少certificate属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Authentication_Attr_O, etSCDIEDAccessPointCheck_Authentication_Attr_O);
                        }
                        
                        m_vn->toElement(PARENT); // return to parent
                    }
                    else
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server元素缺少Authentication子元素", WtoA(iedCache.name).c_str(), name.c_str());
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
                            AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_LDevice_Attr_R, 1); // LDevice元素含有inst等1个属性
                            AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_LDevice_Attr_O, 1); // LDevice元素含有desc等1个属性
                            AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_LDevice_Child, 1); // LDevice元素含有子元素LN0零逻辑节点实例元素、LN其他逻辑节点实例元素、AccessControl访问控制实例元素。LN0为必选元素
                            
                            string inst = WtoA(VtdGetAttributeValue(m_vn, L"inst")).c_str();
                            if (!m_vn->hasAttr(L"inst"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LDevice缺少inst属性", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_LDevice_Attr_R, etSCDIEDAccessPointCheck_LDevice_Attr_R);
                            }
                            if (!m_vn->hasAttr(L"desc"))
                            {
                                string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LDevice缺少desc属性", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
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
                                AddDueElementInfo(bLNParentChecked, bSCDIEDLNCheck_LN0_Attr_R, 3); // LN0元素含有lnClass, InType, inst和desc等4个属性
                                AddDueElementInfo(bLNParentChecked, bSCDIEDLNCheck_LN0_Attr_O, 1); // LN0元素含有desc等1个属性
                                // LN0元素含有子元素DataSet数据集实例元素、ReportControl报告控制实例元素、LogControl日志控制实例元素、DOI数据对象实例元素、
                                // Inputs信息输入实例元素、GSEControlGSE控制实例元素、SampledValueControl采样值控制实例元素、SettingControl定值控制实例元素、
                                // SCLControlSCL控制元素、Log日志元素
                                AddDueElementInfo(bLNParentChecked, bSCDIEDLNCheck_LN0_Child, 10); 

                                if (!m_vn->hasAttr(L"desc"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0元素缺少desc属性", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Attr_O, etSCDIEDLNCheck_LN0_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"inst"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0元素缺少inst属性", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Attr_R, etSCDIEDLNCheck_LN0_Attr_R);
                                }
                                if (!m_vn->hasAttr(L"lnType"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0元素缺少lnType属性", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Attr_R, etSCDIEDLNCheck_LN0_Attr_R);
                                }
                                if (!m_vn->hasAttr(L"lnClass"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0元素缺少lnClass属性", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Attr_R, etSCDIEDLNCheck_LN0_Attr_R);
                                }
                                
                                // child
                                if (m_vn->toElement(FIRST_CHILD, L"DataSet"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0缺少DataSet子元素", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"ReportControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0缺少ReportControl子元素", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"LogControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0缺少LogControl子元素", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"DOI"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0缺少DOI子元素", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"Inputs"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0缺少Inputs子元素", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"GSEControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0缺少GSEControl子元素", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"SampledValueControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0缺少SampledValueControl子元素", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"SettingControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0缺少SettingControl子元素", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"SCLControl"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0缺少SCLControl子元素", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"Log"))
                                {
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]LN0缺少Log子元素", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN0_Child, etSCDIEDLNCheck_LN0_Child_O);
                                }
                                
                                m_vn->toElement(PARENT);
                            }
                            else
                            {
                                string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]Server的子元素LDevice缺少LN0子元素", WtoA(iedCache.name).c_str(), name.c_str(), inst.c_str());
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
                                string msg = "逻辑设备不唯一:IED name=%s, LDevice inst=%s";
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
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server元素缺少LDevice子元素", WtoA(iedCache.name).c_str(), name.c_str());
                        SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Server_Child, etSCDIEDAccessPointCheck_Server_Child);
                    }

                    if (m_vn->toElement(FIRST_CHILD, L"Association"))
                    {
                        bool bSCDIEDAccessPointCheck_Association_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Association_Attr_R").bEnabled;
                        ErrorType etSCDIEDAccessPointCheck_Association_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Association_Attr_R").etValue;
                        bool bSCDIEDAccessPointCheck_Association_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Association_Attr_O").bEnabled;
                        ErrorType etSCDIEDAccessPointCheck_Association_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDAccessPointCheck_Association_Attr_O").etValue;
                        // Association元素含有kind, associationID, iedName, IdInst, InClass, prefix及InInst等7个属性
                        AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_Association_Attr_R, 5);  //  desc="Association元素含有kind,  iedName, ldInst, lnClass, 及lnInst等5个属性"
                        AddDueElementInfo(bParentChecked, bSCDIEDAccessPointCheck_Association_Attr_O, 2); //  desc="Association元素含有associationID,  prefix等2个属性"
                        if (!m_vn->hasAttr(L"kind"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Association缺少kind属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_R, etSCDIEDAccessPointCheck_Association_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"associationID"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Association缺少associationID属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_O, etSCDIEDAccessPointCheck_Association_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"iedName"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Association缺少iedName属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_R, etSCDIEDAccessPointCheck_Association_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"ldInst"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Association缺少ldInst属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_R, etSCDIEDAccessPointCheck_Association_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"lnClass"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Association缺少lnClass属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_R, etSCDIEDAccessPointCheck_Association_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"prefix"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Association缺少prefix属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_O, etSCDIEDAccessPointCheck_Association_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"lnInst"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]Server的子元素Association缺少lnInst属性", WtoA(iedCache.name).c_str(), name.c_str());
                            SCDIEDAccessPointCheck(msg, bSCDIEDAccessPointCheck_Association_Attr_R, etSCDIEDAccessPointCheck_Association_Attr_R);
                        }

                        m_vn->toElement(PARENT);
                    }

                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s]AccessPoint元素缺少子元素Server", WtoA(iedCache.name).c_str(), name.c_str());
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
                    string msg = "AccessPoint不唯一:IED name=%s, AccessPoint name=%s";
                    CGeneralCheck::GetInstance()->Report(row, xml, msg.c_str(),
                        WtoA(iedCache.name).c_str(), WtoA(apName).c_str());
                }

            } while (m_vn->toElement(NEXT_SIBLING, L"AccessPoint"));
            m_vn->toElement(PARENT);
        }
        else
        {
            string msg = "IED元素缺少AccessPoint子元素";
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
                    // 判断子项是否被选中
                    bool bSCDIEDLNCheck_DataSet_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DataSet_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_DataSet_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DataSet_Attr_R").etValue;
                    bool bSCDIEDLNCheck_DataSet_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DataSet_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_DataSet_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DataSet_Attr_O").etValue;
                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DataSet_Attr_R, 1); // DataSet元素含有name等1个属性
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DataSet_Attr_O, 1); // DataSet元素含有desc等1个属性

                    string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DataSet name=%s]DataSet元素缺少name属性", WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_DataSet_Attr_R, etSCDIEDLNCheck_DataSet_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DataSet name=%s]DataSet元素缺少desc属性", WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
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
                        string msg = "数据集存在重复:IED name=%s LDevice inst=%s DataSet name=%s";
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
                // 判断子项是否被选中
                bool bSCDIEDLNCheck_FCDA_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_FCDA_Attr_R").bEnabled;
                ErrorType etSCDIEDLNCheck_FCDA_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_FCDA_Attr_R").etValue;
                bool bSCDIEDLNCheck_FCDA_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_FCDA_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_FCDA_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_FCDA_Attr_O").etValue;
                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_FCDA_Attr_R, 1); // FCDA元素含有fc等1个属性
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_FCDA_Attr_O, 6); // FCDA元素含有IdInst, prefix, InClass, InInst, doName, daName等6个属性
                             
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
                 // 完整性检测
                if (!m_vn->hasAttr(L"ldInst"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDA缺少ldInst属性", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"prefix"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDA缺少prefix属性", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"lnClass"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDA缺少lnClass属性", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"lnInst"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDA缺少lnInst属性", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"doName"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDA缺少doName属性", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"daName"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDA缺少daName属性", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_O, etSCDIEDLNCheck_FCDA_Attr_O);
                }
                if (!m_vn->hasAttr(L"fc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s LDevice inst=%s DataSet name=%s FCDA path=%s]FCDA缺少fc属性", 
                        WtoA(dsCache.iedName).c_str(), WtoA(dsCache.ldInst).c_str(), dsCache.name.c_str(), WtoA(fcdaCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_FCDA_Attr_R, etSCDIEDLNCheck_FCDA_Attr_R);
                }

                FCDAMap::iterator it = dsCache.mapFCDA.find(fcdaCache.key);
                if (it != dsCache.mapFCDA.end())
                {
                    int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
                    string xml = pChecker->GetRowContentByRowNumber(row);
                    string msg = "FCDA 不唯一:IED name=%s LDevice inst=%s DataSet name=%s FCDA=%s";
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
                    // 判断子项是否被选中
                    bool bSCDIEDLNCheck_GSEControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_GSEControl_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_GSEControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_GSEControl_Attr_R").etValue;
                    bool bSCDIEDLNCheck_GSEControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_GSEControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_GSEControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_GSEControl_Attr_O").etValue;
                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // GSEControl元素含有name, desc, datSet, confRev, type及appID等6个属性
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_GSEControl_Attr_R, 2); 
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_GSEControl_Attr_O, 4); 

                    string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControl缺少name属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_GSEControl_Attr_R, etSCDIEDLNCheck_GSEControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControl缺少desc属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_GSEControl_Attr_O, etSCDIEDLNCheck_GSEControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"datSet"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControl缺少datSet属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_GSEControl_Attr_O, etSCDIEDLNCheck_GSEControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"confRev"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControl缺少confRev属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_GSEControl_Attr_O, etSCDIEDLNCheck_GSEControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"type"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControl缺少type属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_GSEControl_Attr_O, etSCDIEDLNCheck_GSEControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"appID"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s GSEControl name=%s]GSEControl缺少appID属性", 
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
                        string msg = "Goose控制块存在重复:IED name=%s LDevice inst=%s ControlBlock name=%s";
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
                    // 判断子项是否被选中
                    bool bSCDIEDLNCheck_ReportControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_ReportControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Attr_R").etValue;
                    bool bSCDIEDLNCheck_ReportControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_ReportControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Attr_O").etValue;
                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // ReportControl元素含有name, desc, datSet, intgPd, rptID, confRev, buffered及bufTime等8个属性
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_ReportControl_Attr_R, 2); //  desc="ReportControl元素含有name, confRev等2个属性"
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_ReportControl_Attr_O, 6);  //  desc="ReportControl元素含有desc, datSet, intgPd, rptID, buffered及bufTime等6个属性"

                    string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl缺少name属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_R, etSCDIEDLNCheck_ReportControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl缺少desc属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"datSet"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl缺少datSet属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"intgPd"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl缺少intgPd属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"rptID"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl缺少rptID属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"confRev"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl缺少confRev属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_R, etSCDIEDLNCheck_ReportControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"buffered"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl缺少buffered属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"bufTime"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl缺少bufTime属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_ReportControl_Attr_O, etSCDIEDLNCheck_ReportControl_Attr_O);
                    }
                    
                    // Child
                    bool bSCDIEDLNCheck_ReportControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Child").bEnabled;
                    ErrorType etSCDIEDLNCheck_ReportControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ReportControl_Child").etValue;
                    // ReportControl元素含有子元素：TrgOps触发选项元素、OptFields选项元素、RptEnabled报告使能元素。其中OptFields元素为必选
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
                                // OptFields元素含有seqNum, timeStamp, dataSet, reasonCode, dataRef, bufOvfl, entryID, configRef及segmentation等9个属性
                                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_OptFields_Attr, 9);
                                if (!m_vn->hasAttr(L"seqNum"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素OptFields元素缺少seqNum属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"timeStamp"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素OptFields元素缺少timeStamp属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"dataSet"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素OptFields元素缺少dataSet属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"reasonCode"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素OptFields元素缺少reasonCode属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"dataRef"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素OptFields元素缺少dataRef属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"bufOvfl"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素OptFields元素缺少bufOvfl属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"entryID"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素OptFields元素缺少entryID属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"configRef"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素OptFields元素缺少configRef属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"segmentation"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素OptFields元素缺少segmentation属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_OptFields_Attr, etSCDIEDLNCheck_OptFields_Attr_O);
                                }

                            }
                            else if (tag == L"TrgOps")
                            {
                                bool bSCDIEDLNCheck_TrgOps_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_TrgOps_Attr_O").bEnabled;
                                ErrorType etSCDIEDLNCheck_TrgOps_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_TrgOps_Attr_O").etValue;
                                 // TrgOps元素含有dchg, qchg, dupd及period等4个属性
                                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_TrgOps_Attr, 4);
                                // CheckTrgOpsAttr(bSCDIEDLNCheck_TrgOps_Attr);
                                if (!m_vn->hasAttr(L"dchg"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素TrgOps元素缺少dchg属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"qchg"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素TrgOps元素缺少qchg属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"dupd"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素TrgOps元素缺少dupd属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"period"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素TrgOps元素缺少period属性", 
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
                                // RptEnabled元素含有desc及max等2个属性
                                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_RptEnabled_Attr_O, 2);
                                // RptEnabled元素含有子元素：ClientLN报告控制块客户端元素
                                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_RptEnabled_Child, 1);
                                if (!m_vn->hasAttr(L"desc"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素RptEnabled元素缺少desc属性", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_RptEnabled_Attr_O, etSCDIEDLNCheck_RptEnabled_Attr_O);
                                }
                                if (!m_vn->hasAttr(L"max"))
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素RptEnabled元素缺少max属性", 
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
                                        // ClientLN元素含有ideName, IdInst, prefix, InClass及InInst等5个属性
                                        AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_ClientLN_Attr_R, 4);
                                        AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_ClientLN_Attr_O, 1); //prefix
                                        if (!m_vn->hasAttr(L"iedName"))
                                        {
                                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl下RptEnabled的子元素ClientLN缺少iedName属性", 
                                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ClientLN_Attr_R, etSCDIEDLNCheck_ClientLN_Attr_R);
                                        }
                                        if (!m_vn->hasAttr(L"ldInst"))
                                        {
                                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl下RptEnabled的子元素ClientLN缺少ldInst属性", 
                                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ClientLN_Attr_R,etSCDIEDLNCheck_ClientLN_Attr_R);
                                        }
                                        if (!m_vn->hasAttr(L"prefix"))
                                        {
                                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl下RptEnabled的子元素ClientLN缺少prefix属性", 
                                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ClientLN_Attr_O, etSCDIEDLNCheck_ClientLN_Attr_O);
                                        }
                                        if (!m_vn->hasAttr(L"lnClass"))
                                        {
                                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl下RptEnabled的子元素ClientLN缺少lnClass属性", 
                                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ClientLN_Attr_R, etSCDIEDLNCheck_ClientLN_Attr_R);
                                        }
                                        if (!m_vn->hasAttr(L"lnInst"))
                                        {
                                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl下RptEnabled的子元素ClientLN缺少lnInst属性", 
                                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ClientLN_Attr_R, etSCDIEDLNCheck_ClientLN_Attr_R);
                                        }

                                    } while (m_vn->toElement(NEXT_SIBLING, L"ClientLN"));
                                    m_vn->toElement(PARENT);
                                }
                                else
                                {
                                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl的子元素RptEnabled缺少ClientLN子元素", 
                                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_RptEnabled_Child, etSCDIEDLNCheck_RptEnabled_Child_O);
                                }

                            }

                        } while (m_vn->toElement(NEXT_SIBLING));
                        m_vn->toElement(PARENT);
                    }

                    if (!bHave_ReportControl_OptFields)
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ReportControl name=%s]ReportControl元素缺少OptFields子元素", 
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
                        string msg = "Report控制块存在重复:IED name=%s LDevice inst=%s ControlBlock name=%s";
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
                    // 判断子项是否被选中
                    bool bSCDIEDLNCheck_LogControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_LogControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Attr_R").etValue;
                    bool bSCDIEDLNCheck_LogControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_LogControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Attr_O").etValue;
                    bool bSCDIEDLNCheck_LogControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Child").bEnabled;
                    ErrorType etSCDIEDLNCheck_LogControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LogControl_Child").etValue;
                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // LogControl元素含有name, desc, datSet, intgPd, logName, logEna及reasonCode等7个属性
                    // LogControl元素含有子元素：TrgOps触发选项元素
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_LogControl_Attr_R, 2);  //  desc="LogControl元素含有name, logName等2个属性"
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_LogControl_Attr_O, 5);  //  desc="LogControl元素含有desc, datSet, intgPd, logEna及reasonCode等5个属性"
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_LogControl_Child, 1); 

                    string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl缺少name属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_R, etSCDIEDLNCheck_LogControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl缺少desc属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_O, etSCDIEDLNCheck_LogControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"datSet"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl缺少datSet属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_O, etSCDIEDLNCheck_LogControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"intgPd"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl缺少intgPd属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_O, etSCDIEDLNCheck_LogControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"logName"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl缺少logName属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_R, etSCDIEDLNCheck_LogControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"logEna"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl缺少logEna属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_O, etSCDIEDLNCheck_LogControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"reasonCode"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl缺少reasonCode属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_LogControl_Attr_O, etSCDIEDLNCheck_LogControl_Attr_O);
                    }

                    // child
                    if (m_vn->toElement(FIRST_CHILD, L"TrgOps"))
                    {
                        bool bSCDIEDLNCheck_TrgOps_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_TrgOps_Attr").bEnabled;
                        ErrorType etSCDIEDLNCheck_TrgOps_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_TrgOps_Attr").etValue;
                        // TrgOps元素含有dchg, qchg, dupd及period等4个属性
                        AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_TrgOps_Attr, 4);
                        // CheckTrgOpsAttr(bSCDIEDLNCheck_TrgOps_Attr);
                        if (!m_vn->hasAttr(L"dchg"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl的TrgOps子元素缺少dchg属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr);
                        }
                        if (!m_vn->hasAttr(L"qchg"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl的TrgOps子元素缺少qchg属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr);
                        }
                        if (!m_vn->hasAttr(L"dupd"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl的TrgOps子元素缺少dupd属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr);
                        }
                        if (!m_vn->hasAttr(L"period"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl的TrgOps子元素缺少period属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_TrgOps_Attr, etSCDIEDLNCheck_TrgOps_Attr);
                        }
                        m_vn->toElement(PARENT);
                    }
                    else
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LogControl name=%s]LogControl缺少TrgOps子元素", 
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
                        string msg = "日志控制块存在重复:IED name=%s LDevice inst=%s ControlBlock name=%s";
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
        string msg = "TrgOps元素缺少dchg属性";
        SCDIEDLNCheck(msg, bSubItemChecked, etValue);
    }
    if (!m_vn->hasAttr(L"qchg"))
    {
        string msg = "TrgOps元素缺少qchg属性";
        SCDIEDLNCheck(msg, bSubItemChecked, etValue);
    }
    if (!m_vn->hasAttr(L"dupd"))
    {
        string msg = "TrgOps元素缺少dupd属性";
        SCDIEDLNCheck(msg, bSubItemChecked, etValue);
    }
    if (!m_vn->hasAttr(L"period"))
    {
        string msg = "TrgOps元素缺少period属性";
        SCDIEDLNCheck(msg, bSubItemChecked, etValue);
    }
    return true;
}

// 添加SettingControl和SCLControl
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
                    // 判断子项是否被选中
                    bool bSCDIEDLNCheck_SCLControl_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SCLControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_SCLControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SCLControl_Attr_O").etValue;

                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // SCLControl元素含有desc属性
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SCLControl_Attr, 1); 

                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]SCLControl缺少desc属性", 
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
                        string msg = "日志控制块存在重复:IED name=%s LDevice inst=%s ControlBlock name=%s";
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
                    // 判断子项是否被选中
                    bool bSCDIEDLNCheck_SettingControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SettingControl_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_SettingControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SettingControl_Attr_R").etValue;

                    bool bSCDIEDLNCheck_SettingControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SettingControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_SettingControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SettingControl_Attr_O").etValue;

                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // SettingControl元素含有desc, numOfSGs及actSG等3个属性
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SettingControl_Attr_R, 1); 
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SettingControl_Attr_O, 2); 

                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]SettingControl缺少desc属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SettingControl_Attr_R, etSCDIEDLNCheck_SettingControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"numOfSGs"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]SettingControl缺少numOfSGs属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SettingControl_Attr_O, etSCDIEDLNCheck_SettingControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"actSG"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]SettingControl缺少actSG属性", 
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
                    //    string msg = "日志控制块存在重复:IED name=%s LDevice inst=%s ControlBlock name=%s";
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
                    // 判断子项是否被选中
                    bool bSCDIEDLNCheck_SampledValueControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Attr_R").bEnabled;
                    ErrorType etSCDIEDLNCheck_SampledValueControl_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Attr_R").etValue;

                    bool bSCDIEDLNCheck_SampledValueControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Attr_O").bEnabled;
                    ErrorType etSCDIEDLNCheck_SampledValueControl_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Attr_O").etValue;

                    bool bSCDIEDLNCheck_SampledValueControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Child").bEnabled;
                    ErrorType etSCDIEDLNCheck_SampledValueControl_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SampledValueControl_Child").etValue;

                    bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                    // SampledValueControl元素含有name, desc, datSet, confRev, smvID, multicast, smpRate及nofASDU等8个属性
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SampledValueControl_Attr_R, 4); //  desc="SampledValueControl元素含有name, smvID, smpRate及nofASDU等4个属性"
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SampledValueControl_Attr_O, 4); 
                    // SampledValueControl元素含有子元素SmvOpts采样值选项元素。此元素为必选
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SampledValueControl_Child, 1); 

                    string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl缺少name属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_R, etSCDIEDLNCheck_SampledValueControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"desc"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl缺少desc属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_O, etSCDIEDLNCheck_SampledValueControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"datSet"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl缺少datSet属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_O, etSCDIEDLNCheck_SampledValueControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"confRev"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl缺少confRev属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_O, etSCDIEDLNCheck_SampledValueControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"smvID"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl缺少smvID属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_R, etSCDIEDLNCheck_SampledValueControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"multicast"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl缺少multicast属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_O, etSCDIEDLNCheck_SampledValueControl_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"smpRate"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl缺少smpRate属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_R, etSCDIEDLNCheck_SampledValueControl_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"nofASDU"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl缺少nofASDU属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_SampledValueControl_Attr_R, etSCDIEDLNCheck_SampledValueControl_Attr_R);
                    }
                    
                    if (m_vn->toElement(FIRST_CHILD, L"SmvOpts"))
                    {
                        bool bSCDIEDLNCheck_SmvOpts_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SmvOpts_Attr_O").bEnabled;
                        ErrorType etSCDIEDLNCheck_SmvOpts_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SmvOpts_Attr_O").etValue;

                        // SmvOpts元素含有refreshTime, sampleSynchronized, sampleRate, security及dataRef等5个属性
                         AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_SmvOpts_Attr, 5); 
                         if (!m_vn->hasAttr(L"refreshTime"))
                         {
                             string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl的SmvOpts子元素缺少refreshTime属性", 
                                 WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                             SCDIEDLNCheck(msg, bSCDIEDLNCheck_SmvOpts_Attr, etSCDIEDLNCheck_SmvOpts_Attr_O);
                         }
                         if (!m_vn->hasAttr(L"sampleSynchronized"))
                         {
                             string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl的SmvOpts子元素缺少sampleSynchronized属性", 
                                 WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                             SCDIEDLNCheck(msg, bSCDIEDLNCheck_SmvOpts_Attr, etSCDIEDLNCheck_SmvOpts_Attr_O);
                         }
                         if (!m_vn->hasAttr(L"sampleRate"))
                         {
                             string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl的SmvOpts子元素缺少sampleRate属性", 
                                 WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                             SCDIEDLNCheck(msg, bSCDIEDLNCheck_SmvOpts_Attr, etSCDIEDLNCheck_SmvOpts_Attr_O);
                         }
                         if (!m_vn->hasAttr(L"security"))
                         {
                             string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl的SmvOpts子元素缺少security属性", 
                                 WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                             SCDIEDLNCheck(msg, bSCDIEDLNCheck_SmvOpts_Attr, etSCDIEDLNCheck_SmvOpts_Attr_O);
                         }
                         if (!m_vn->hasAttr(L"dataRef"))
                         {
                             string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl的SmvOpts子元素缺少dataRef属性", 
                                 WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), name.c_str());
                             SCDIEDLNCheck(msg, bSCDIEDLNCheck_SmvOpts_Attr, etSCDIEDLNCheck_SmvOpts_Attr_O);
                         }

                        m_vn->toElement(PARENT);
                    }
                    else
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SampledValueControl name=%s]SampledValueControl缺少SmvOpts子元素", 
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
                        string msg = "SampledValueControl存在重复:IED name=%s LDevice inst=%s ControlBlock name=%s";
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
                
                // 判断子项是否被选中
                bool bSCDIEDLNCheck_Inputs_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Inputs_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_Inputs_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Inputs_Attr_O").etValue;

                bool bSCDIEDLNCheck_Inputs_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Inputs_Child").bEnabled;   
                ErrorType etSCDIEDLNCheck_Inputs_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Inputs_Child").etValue;

                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                // Inputs元素含有desc属性
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_Inputs_Attr, 1);
                // Inputs元素含有子元素ExRef外部信息元素
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_Inputs_Child, 1);

                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]Inputs元素缺少desc属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_Inputs_Attr, etSCDIEDLNCheck_Inputs_Attr_O);
                }

                if (m_vn->toElement(FIRST_CHILD, L"ExtRef"))
                {
                    do 
                    {
                        bool bSCDIEDLNCheck_ExtRef_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ExtRef_Attr_O").bEnabled;
                        ErrorType etSCDIEDLNCheck_ExtRef_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_ExtRef_Attr_O").etValue;

                        // ExtRef元素含有iedName, IdInst, prefix, InClass, InInst, doName, daName及intAddr等8个属性。
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
                        // 处理短地址中包含物理端口信息的情况
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
                        // 放入结构中便于统计虚回路
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

                        // 完整性检测
                        if (!m_vn->hasAttr(L"iedName"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRef元素缺少iedName属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"ldInst"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRef元素缺少ldInst属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"prefix"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRef元素缺少prefix属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"lnClass"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRef元素缺少lnClass属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"lnInst"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRef元素缺少lnInst属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"doName"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRef元素缺少doName属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"daName"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRef元素缺少daName属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"intAddr"))
                        {
                            string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s ExtRef intAddr=%s]ExtRef元素缺少intAddr属性", 
                                WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(intAddr).c_str());
                            SCDIEDLNCheck(msg, bSCDIEDLNCheck_ExtRef_Attr, etSCDIEDLNCheck_ExtRef_Attr_O);
                        }

                        wstring key = inputCache.iedName + inputCache.outSignal + inputCache.inSignal;
                        InputsMap::iterator it = ldCache.mapInputs.find(key);
                        if (it != ldCache.mapInputs.end())
                        {
                            int row = pChecker->CalcRowNumberFromOffset(offset);
                            string xml = pChecker->GetRowContentByOffset(offset);
                            string msg = "虚回路存在重复:IED name=%s LDevice inst=%s Inputs 内部地址intAddr=%s 外部索引ExtRef=%s";
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
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s]Inputs元素缺少ExtRef子元素",
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
        // m_SCDCache.mapAssoIED[out].insert(in); 防止重复报间隔链接错误
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
                // 判断子项是否被选中
                bool bSCDIEDLNCheck_LN_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN_Attr_R").bEnabled;
                ErrorType etSCDIEDLNCheck_LN_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN_Attr_R").etValue;

                bool bSCDIEDLNCheck_LN_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_LN_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_LN_Attr_O").etValue;

                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                // LN元素含有InClass, InType, inst, desc及prefix等5个属性
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_LN_Attr_R, 3); //  desc="LN元素含有InClass, InType, inst等3个属性"
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_LN_Attr_O, 2); 

                wstring lnClass = VtdGetAttributeValue(m_vn, L"lnClass");
                wstring prefix = VtdGetAttributeValue(m_vn, L"prefix");
                wstring inst = VtdGetAttributeValue(m_vn, L"inst");


                string key = WtoA(prefix + lnClass +inst);
                if (!m_vn->hasAttr(L"lnClass"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s]LN元素缺少lnClass属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), key.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN_Attr_R, etSCDIEDLNCheck_LN_Attr_R);
                }
                if (!m_vn->hasAttr(L"lnType"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s]LN元素缺少lnType属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), key.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN_Attr_R, etSCDIEDLNCheck_LN_Attr_R);
                }
                if (!m_vn->hasAttr(L"inst"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s]LN元素缺少inst属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), key.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN_Attr_R, etSCDIEDLNCheck_LN_Attr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s]LN元素缺少desc属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), key.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_LN_Attr_O, etSCDIEDLNCheck_LN_Attr_O);
                }
                if (!m_vn->hasAttr(L"prefix"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s]LN元素缺少prefix属性", 
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
            string msg = "当前节点下Ln实例不唯一:IED name=%s LDevice inst=%s LN=%s";
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
                // 判断子项是否被选中
                bool bSCDIEDLNCheck_DOI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Attr_R").bEnabled;
                ErrorType etSCDIEDLNCheck_DOI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Attr_R").etValue;

                bool bSCDIEDLNCheck_DOI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_DOI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Attr_O").etValue;

                bool bSCDIEDLNCheck_DOI_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Child_O").bEnabled;
                ErrorType etSCDIEDLNCheck_DOI_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DOI_Child_O").etValue;

                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                // DOI元素含有desc, name, ix及accessControl等4个属性
                // DOI元素含有子元素：DAI数据属性实例元素、SDI子数据实例元素。均是必选元素
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DOI_Attr_R, 1); //  desc="DOI元素含name等1个属性"
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DOI_Attr_O, 3);
                AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_DOI_Child, 2);

                string doiName = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOI元素缺少desc属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(lnNode.key).c_str(), doiName.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DOI_Attr_O, etSCDIEDLNCheck_DOI_Attr_O);
                }
                if (!m_vn->hasAttr(L"name"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOI元素缺少name属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(lnNode.key).c_str(), doiName.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DOI_Attr_R, etSCDIEDLNCheck_DOI_Attr_R);
                }
                if (!m_vn->hasAttr(L"ix"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOI元素缺少ix属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(lnNode.key).c_str(), doiName.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DOI_Attr_O, etSCDIEDLNCheck_DOI_Attr_O);
                }
                if (!m_vn->hasAttr(L"accessControl"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOI元素缺少accessControl属性", 
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
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOI元素缺少DAI子元素", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(lnNode.key).c_str(), doiName.c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DOI_Child, etSCDIEDLNCheck_DOI_Child_O);
                }
                if (m_vn->toElement(FIRST_CHILD, L"SDI"))
                {
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s LN path=%s DOI name=%s]DOI元素缺少SDI子元素", 
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
                    string msg = "当前节点下DOI实例不唯一:IED name=%s LDevice inst=%s LN=%s DOI name=%s";
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
                // 判断子项是否被选中
                bool bSCDIEDLNCheck_DAI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Attr_R").bEnabled;
                ErrorType etSCDIEDLNCheck_DAI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Attr_R").etValue;

                bool bSCDIEDLNCheck_DAI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_DAI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Attr_O").etValue;

                bool bSCDIEDLNCheck_DAI_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Child_O").bEnabled;
                ErrorType etSCDIEDLNCheck_DAI_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_DAI_Child_O").etValue;

                bool bSCDIEDLNCheck_Val_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Val_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_Val_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_Val_Attr_O").etValue;

                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                // DAI元素含有desc, name, sAddr, valKind及ix等5个属性
                // DAI元素含有子元素：Val数据值元素
                
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
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAI缺少desc属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Attr_O, etSCDIEDLNCheck_DAI_Attr_O);
                }
                if (!m_vn->hasAttr(L"name"))
                {
                    string msg =  FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAI缺少name属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Attr_R, etSCDIEDLNCheck_DAI_Attr_R);
                }
                if (!m_vn->hasAttr(L"sAddr"))
                {
                    string msg =  FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAI缺少sAddr属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Attr_O, etSCDIEDLNCheck_DAI_Attr_O);
                }
                if (!m_vn->hasAttr(L"valKind"))
                {
                    string msg =  FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAI缺少valKind属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Attr_O, etSCDIEDLNCheck_DAI_Attr_O);
                }
                if (!m_vn->hasAttr(L"ix"))
                {
                    string msg =  FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAI缺少ix属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Attr_O, etSCDIEDLNCheck_DAI_Attr_O);
                }

                // child
                if (m_vn->toElement(FIRST_CHILD, L"Val"))
                {
                    // Val元素含有sGroup属性
                    AddDueElementInfo(bParentChecked, bSCDIEDLNCheck_Val_Attr, 1);
                    if (!m_vn->hasAttr(L"sGroup"))
                    {
                        string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAI子元素Val缺少sGroup属性", 
                            WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                        SCDIEDLNCheck(msg, bSCDIEDLNCheck_Val_Attr, etSCDIEDLNCheck_Val_Attr_O);
                    }
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s DAI name=%s]DAI缺少Val子元素", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(daiCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_DAI_Child, etSCDIEDLNCheck_DAI_Child_O);
                }

                LogicalNodesInstancesMap::iterator it = ldCache.mapSignalInstance.find(daiCache.key);
                if (it != ldCache.mapSignalInstance.end())
                {
                    int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
                    string xml = pChecker->GetRowContentByRowNumber(row);
                    string msg = "当前节点下DAI实例不唯一:IED name=%s LDevice inst=%s LN=%s DOI name=%s DAI name=%s";
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
                // 判断子项是否被选中
                bool bSCDIEDLNCheck_SDI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Attr_R").bEnabled;
                ErrorType etSCDIEDLNCheck_SDI_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Attr_R").etValue;

                bool bSCDIEDLNCheck_SDI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Attr_O").bEnabled;
                ErrorType etSCDIEDLNCheck_SDI_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Attr_O").etValue;

                bool bSCDIEDLNCheck_SDI_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Child_O").bEnabled;
                ErrorType etSCDIEDLNCheck_SDI_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIEDLNCheck_SDI_Child_O").etValue;

                bool bParentChecked = CSCDIEDLNCheck::GetInstance()->IsEnabled();
                // SDI元素含有desc, name及ix等3个属性
                // SDI元素含有子元素：DAI数据属性实例元素、SDI子数据实例元素。均是必选元素
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
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SDI name=%s]SDI元素缺少desc属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(sdiInstanceCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_SDI_Attr_O, etSCDIEDLNCheck_SDI_Attr_O);
                }
                if (!m_vn->hasAttr(L"name"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SDI name=%s]SDI元素缺少name属性", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(sdiInstanceCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_SDI_Attr_R, etSCDIEDLNCheck_SDI_Attr_R);
                }
                if (!m_vn->hasAttr(L"ix"))
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SDI name=%s]SDI元素缺少ix属性", 
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
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SDI name=%s]SDI元素缺少DAI子元素", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(sdiInstanceCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_SDI_Child, etSCDIEDLNCheck_SDI_Child_O);
                }
                if (m_vn->toElement(FIRST_CHILD, L"SDI"))
                {
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[IED name=%s AccessPoint name=%s LDevice inst=%s SDI name=%s]SDI元素缺少SDI子元素", 
                        WtoA(ldCache.iedName).c_str(), WtoA(ldCache.apName).c_str(), WtoA(ldCache.inst).c_str(), WtoA(sdiInstanceCache.key).c_str());
                    SCDIEDLNCheck(msg, bSCDIEDLNCheck_SDI_Child, etSCDIEDLNCheck_SDI_Child_O);
                }

                LogicalNodesInstancesMap::iterator it = ldCache.mapSignalInstance.find(sdiInstanceCache.key);
                if (it != ldCache.mapSignalInstance.end())
                {
                    int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
                    string xml = WtoA(GetXMLContent());
                    string msg = "当前节点下SDI实例不唯一:IED name=%s LDevice inst=%s LN=%s DOI name=%s SDI name=%s";
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
                // 判断子项是否被选中
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Attr_R").bEnabled;
                ErrorType etSCDHeaderCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Attr_R").etValue;

                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Attr_O").bEnabled;
                ErrorType etSCDHeaderCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Attr_O").etValue;

                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Child").bEnabled;
                ErrorType etSCDHeaderCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Child").etValue;

                bool bHistoryChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_History_Child").bEnabled; 
                ErrorType etSCDHeaderCheck_History_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_History_Child").etValue;

                bool bParentChecked = CSCDHeaderCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 1); // Header元素是否包括ID1个属性
                AddDueElementInfo(bParentChecked, bAttr_O, 4); // Header元素是否包括Version、reversion、toolID、namestructure 4个属性
                AddDueElementInfo(bParentChecked, bChild, 1); // Header元素是否至少包括History子元素

                if (m_vn->hasAttr(L"id"))
                {
                    m_SCDCache.header.id = VtdGetAttributeValue(m_vn, L"id");
                } 
                else
                {
                    string msg = "Header元素缺少id属性";
                    SCDHeaderCheck(msg, bAttr_R, etSCDHeaderCheck_Attr_R);
                }
                if (m_vn->hasAttr(L"version"))
                {
                    m_SCDCache.header.version = VtdGetAttributeValue(m_vn, L"version");
                } 
                else
                {
                    string msg = "Header元素缺少version属性";
                    SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Attr_O);
                }
                if (m_vn->hasAttr(L"revision"))
                {
                    m_SCDCache.header.reversion = VtdGetAttributeValue(m_vn, L"revision");
                } 
                else
                {
                    string msg = "Header元素缺少revision属性";
                    SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Attr_O);
                }
                if (m_vn->hasAttr(L"toolID"))
                {
                    m_SCDCache.header.toolID = VtdGetAttributeValue(m_vn, L"toolID");
                } 
                else
                {
                    string msg = "Header元素缺少toolID属性";
                    SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Attr_O);
                }
                if (m_vn->hasAttr(L"nameStructure"))
                {
                    m_SCDCache.header.namestructure = VtdGetAttributeValue(m_vn, L"nameStructure");
                } 
                else
                {
                    string msg = "Header元素缺少nameStructure属性";
                    SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Attr_O);
                }

                // 子元素
                bool bHaveHistory = false;

                if (m_vn->toElement(FIRST_CHILD))
                {
                    do 
                    {
                        wstring tag = VtdGetElementName(m_vn);
                        if (tag == L"History")
                        {
                            bHaveHistory = true;
                            AddDueElementInfo(bParentChecked, bHistoryChild, 1); // History是否含有Hitem子元素

                            // 是否含有Hitem
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
                                string msg = "History元素下缺少Hitem子元素";
                                SCDHeaderCheck(msg, bHistoryChild, etSCDHeaderCheck_History_Child);
                            }
                        }
                        else
                        {
                            //TODO
                        }

                    } while (m_vn->toElement(NEXT_SIBLING));
                    // 返回父元素
                    m_vn->toElement(PARENT);
                }
                if (!bHaveHistory)
                {
                    string msg = "Header元素下缺少History子元素";
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
          // 判断子项是否被选中
         bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Hitem_Attr_R").bEnabled;
         ErrorType etSCDHeaderCheck_Hitem_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Hitem_Attr_R").etValue;

         bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Hitem_Attr_O").bEnabled;
         ErrorType etSCDHeaderCheck_Hitem_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDHeaderCheck_Hitem_Attr_O").etValue;

         bool bParentChecked = CSCDHeaderCheck::GetInstance()->IsEnabled();
         AddDueElementInfo(bParentChecked, bAttr_R, 3); // “Hitem”是否包括 “Version”、“reversion”、“when”3个属性
         AddDueElementInfo(bParentChecked, bAttr_O, 3); // “Hitem”是否包括 “who”、“what”及“why”3个属性
         
         HitemCache hitem;
         if (m_vn->hasAttr(L"version"))
         {
             hitem.version = VtdGetAttributeValue(m_vn, L"version");
         } 
         else
         {
             string msg = "Hitem元素缺少version属性";
             SCDHeaderCheck(msg, bAttr_R, etSCDHeaderCheck_Hitem_Attr_R);
         }

         if (m_vn->hasAttr(L"revision"))
         {
             hitem.reversion = VtdGetAttributeValue(m_vn, L"revision");
         } 
         else
         {
             string msg = "Hitem元素缺少revision属性";
             SCDHeaderCheck(msg, bAttr_R, etSCDHeaderCheck_Hitem_Attr_R);
         }

         if (m_vn->hasAttr(L"when"))
         {
             hitem.when = VtdGetAttributeValue(m_vn, L"when");
         } 
         else
         {
             string msg = "Hitem元素缺少when属性";
             SCDHeaderCheck(msg, bAttr_R, etSCDHeaderCheck_Hitem_Attr_R);
         }

         if (m_vn->hasAttr(L"who"))
         {
             hitem.who = VtdGetAttributeValue(m_vn, L"who");
         } 
         else
         {
             string msg = "Hitem元素缺少who属性";
             SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Hitem_Attr_O);
         }

         if (m_vn->hasAttr(L"what"))
         {
             hitem.what = VtdGetAttributeValue(m_vn, L"what");
         } 
         else
         {
             string msg = "Hitem元素缺少what属性";
             SCDHeaderCheck(msg, bAttr_O, etSCDHeaderCheck_Hitem_Attr_O);
         }

         if (m_vn->hasAttr(L"why"))
         {
             hitem.why = VtdGetAttributeValue(m_vn, L"why");
         } 
         else
         {
             string msg = "Hitem元素缺少why属性";
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
            // 判断子项是否被选中
            bool bAttr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDCommunicationCheck_Attr_O").bEnabled;
            ErrorType etSCDCommunicationCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDCommunicationCheck_Attr_O").etValue;
            bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDCommunicationCheck_Child").bEnabled;
            ErrorType etSCDCommunicationCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDCommunicationCheck_Child").etValue;
            bool bParentChecked = CSCDCommunicationCheck::GetInstance()->IsEnabled();
            AddDueElementInfo(bParentChecked, bAttr, 1); // Communication元素是否含有desc 属性
            AddDueElementInfo(bParentChecked, bChild, 1); // Communication元素是否含有一个必选的子元素SubNetwork子网

            if (!m_vn->hasAttr(L"desc"))
            {
                string msg = "Communication元素缺少desc属性";
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
                string msg = "Communication元素缺少SubNetwork子元素";
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
        // 判断子项是否被选中
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
        AddDueElementInfo(bParentChecked, bAttr_R, 1); // SubNetwork元素是否含有name等1个属性
        AddDueElementInfo(bParentChecked, bAttr_O, 2); // SubNetwork元素是否含有desc和type 等2个属性
        AddDueElementInfo(bParentChecked, bChild, 1); // SubNetwork元素含有子元素 BitRate比特率、ConnectedAP连接访问点。其中ConnectedAP为必选。
        
        SubNetWorkCache subNetCache;
        if (m_vn->hasAttr(L"name"))
        {
            subNetCache.subNetWorkName = VtdGetAttributeValue(m_vn, L"name");
        } 
        else
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWork元素缺少name属性", WtoA(subNetCache.subNetWorkName).c_str());
            SCDSubNetWorkCheck(msg, bAttr_R, etSCDSubNetWorkCheck_Attr_R);
        }
        long offset = GetOffsetOfVTD();
        
        if (!m_vn->hasAttr(L"desc"))
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWork元素缺少desc属性", WtoA(subNetCache.subNetWorkName).c_str());
            SCDSubNetWorkCheck(msg, bAttr_O, etSCDSubNetWorkCheck_Attr_O);
        }
        if (!m_vn->hasAttr(L"type"))
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWork元素缺少type属性", WtoA(subNetCache.subNetWorkName).c_str());
            SCDSubNetWorkCheck(msg, bAttr_O, etSCDSubNetWorkCheck_Attr_O);
        } 
        SubFormatCheck(subNetCache.subNetWorkName);
        
        // 是否有ConnectedAP
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
                else if (tag == L"BitRate") // BitRate子元素
                {
                    AddDueElementInfo(bParentChecked, bBitRate_Attr_R, 1);
                    AddDueElementInfo(bParentChecked, bBitRate_Attr_O, 1);
                    if (!m_vn->hasAttr(L"multiplier"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWork下BitRate元素缺少multiplier属性", WtoA(subNetCache.subNetWorkName).c_str());
                        SCDSubNetWorkCheck(msg, bBitRate_Attr_O, etSCDSubNetWorkCheck_BitRate_Attr_O);
                    }
                    if (!m_vn->hasAttr(L"unit"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWork下BitRate元素缺少unit属性", WtoA(subNetCache.subNetWorkName).c_str());
                        SCDSubNetWorkCheck(msg, bBitRate_Attr_R, etSCDSubNetWorkCheck_BitRate_Attr_R);
                    }
                }
            } while (m_vn->toElement(NEXT_SIBLING));
            // 返回父元素
            m_vn->toElement(PARENT);
        }
        // 子元素告警
        if (!bHaveConnectedAP)
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s]SubNetWork元素缺少ConnectedAP元素", WtoA(subNetCache.subNetWorkName).c_str());
            SCDSubNetWorkCheck(msg, bChild, etSCDSubNetWorkCheck_Child);
        }        


        SubNetWorkMap::iterator it = m_SCDCache.mapSubNet.find(subNetCache.subNetWorkName);
        if (it != m_SCDCache.mapSubNet.end())
        {
            string msg = "子网名称不唯一:SubnetWork name=%s";
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
        // 判断子项是否被选中
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDConnectedAPCheck_Attr_R").bEnabled;
        ErrorType etSCDConnectedAPCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDConnectedAPCheck_Attr_R").etValue;
        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDConnectedAPCheck_Attr_O").bEnabled;
        ErrorType etSCDConnectedAPCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDConnectedAPCheck_Attr_O").etValue;
        bool bParentChecked = CSCDConnectedAPCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 2); // ConnectedAP元素含有iedName 和apName等2个属性。
        AddDueElementInfo(bParentChecked, bAttr_O, 1); // ConnectedAP元素含有desc等1个属性。

        if (!m_vn->hasAttr(L"iedName"))
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]ConnectedAP元素缺少iedName属性", 
                WtoA(subNetCache.subNetWorkName).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"iedName")).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"apName")).c_str());
            SCDConnectedAPCheck(msg, bAttr_R, etSCDConnectedAPCheck_Attr_R);
        }
        if (!m_vn->hasAttr(L"desc"))
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]ConnectedAP元素缺少desc属性", 
                WtoA(subNetCache.subNetWorkName).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"iedName")).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"apName")).c_str());
            SCDConnectedAPCheck(msg, bAttr_O, etSCDConnectedAPCheck_Attr_O);
        }
        if (!m_vn->hasAttr(L"apName"))
        {
            string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]ConnectedAP元素缺少apName属性", 
                WtoA(subNetCache.subNetWorkName).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"iedName")).c_str(), WtoA(VtdGetAttributeValue(m_vn, L"apName")).c_str());
            SCDConnectedAPCheck(msg, bAttr_R, etSCDConnectedAPCheck_Attr_R);
        }

        wstring key = VtdGetAttributeValue(m_vn, L"iedName") + L"." + VtdGetAttributeValue(m_vn, L"apName");
        // 在当前子网中查找
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
        // 判断子项是否被选中
        bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIPAddressCheck_Child").bEnabled;
        ErrorType etSCDIPAddressCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIPAddressCheck_Child").etValue;

        bool bP_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIPAddressCheck_P_Attr").bEnabled;
        ErrorType etSCDIPAddressCheck_P_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDIPAddressCheck_P_Attr").etValue;

        bool bParentChecked = CSCDIPAddressCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bChild, 1); // Address元素是否含有一个必选参数P
        AddDueElementInfo(bParentChecked, bP_Attr, 2); // P_type包括IP, IP_SUBNET,

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
                    string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]Address缺少type为IP的P节点", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                    SCDIPAddressCheck(msg, bP_Attr, etSCDIPAddressCheck_P_Attr);
                }
                if (!bHaveSubNet)
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]Address缺少type为IP-SUBNET的P节点", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                    SCDIPAddressCheck(msg, bP_Attr, etSCDIPAddressCheck_P_Attr);
                }
            }
            else
            {
                string msg = FormatXMLMessage("[SubNetWork name=%s ConnectedAP iedName=%s apName=%s]Address下缺少P元素", 
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
                string msg = "IP地址配置不一致:IED name=%s AP name=%s";
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
                // 判断子项是否被选中
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
                AddDueElementInfo(bParentChecked, bAttr_R, 2); // GSE元素含有IdInst及cbName等2个属性
                AddDueElementInfo(bParentChecked, bAttr_O, 1); // GSE元素含有desc等3个属性
                // AddDueElementInfo(bParentChecked, bChild, 3); // GSE元素应含有子元素Address、MinTime、MaxTime 均为可选。

                

                ControlBlockAddressCache cbAddrCache;
                cbAddrCache.ldInst = VtdGetAttributeValue(m_vn, L"ldInst");
                cbAddrCache.cbName = VtdGetAttributeValue(m_vn, L"cbName");
                cbAddrCache.key = cbAddrCache.ldInst + L"." + cbAddrCache.cbName;

                if (!m_vn->hasAttr(L"ldInst"))
                {
                    string msg =  FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSE元素下缺少ldInst属性", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDGSECheck(msg, bAttr_R, etSCDGSECheck_Attr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSE元素下缺少desc属性", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDGSECheck(msg, bAttr_O, etSCDGSECheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"cbName"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSE元素下缺少cbName属性", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDGSECheck(msg, bAttr_R, etSCDGSECheck_Attr_R);
                }
               

                int offset = GetOffsetOfVTD();
                cbAddrCache.offset = offset;

                if (m_vn->toElement(FIRST_CHILD, L"Address"))
                {
                    AddDueElementInfo(bParentChecked, bAddress_Child, 1); // Address元素是否含有一个必选参数P
                    AddDueElementInfo(bParentChecked, bP_Attr, 4); // P_type应含有MAC_Address, APPID, VLAN_PRIORITY和VLAN_ID等4种类型

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
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Address缺少MAC-Address信息", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDGSECheck(msg, bP_Attr, etSCDGSECheck_P_Attr);
                        }
                        if (!bHaveAppID)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Address缺少AppID信息", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDGSECheck(msg, bP_Attr, etSCDGSECheck_P_Attr);
                        }
                        if (!bHaveVLanID)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Address缺少VLanID信息", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDGSECheck(msg, bP_Attr, etSCDGSECheck_P_Attr);
                        }
                        if (!bHaveVLanPriority)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Address缺少VLanPriority信息", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDGSECheck(msg, bP_Attr, etSCDGSECheck_P_Attr);
                        }
                    }
                    else
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSE的Address子元素缺少P子元素", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            
                        SCDGSECheck(msg, bAddress_Child, etSCDGSECheck_Address_Child);
                    }
                    m_vn->toElement(PARENT);
                }
                //else
                //{
                //    string msg = "GSE下缺少Address子元素";
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
                    AddDueElementInfo(bParentChecked, bMinTime_Attr, 2); // MaxTime元素含有unit和multiplier等2个属性
                    if (!m_vn->hasAttr(L"unit"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSE元素下MinTime元素缺少unit属性", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                        SCDGSECheck(msg, bMinTime_Attr, etSCDGSECheck_MinTime_Attr);
                    }
                    if (!m_vn->hasAttr(L"multiplier"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSE元素下MinTime元素缺少multiplier属性", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                        SCDGSECheck(msg, bMinTime_Attr, etSCDGSECheck_MinTime_Attr);
                    }

                    cbAddrCache.MinTime = VtdGetTextValue(m_vn);
                    m_vn->toElement(PARENT);
                }
                //else
                //{
                //    string msg = "GSE下缺少MinTime子元素";
                //    SCDGSECheck(msg, bChild);
                //}

                if (m_vn->toElement(FIRST_CHILD, L"MaxTime"))
                {
                    AddDueElementInfo(bParentChecked, bMaxTime_Attr, 2); // MinTime元素含有unit和multiplier等2个属性
                    if (!m_vn->hasAttr(L"unit"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSE元素下MaxTime元素缺少unit属性", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                        SCDGSECheck(msg, bMaxTime_Attr, etSCDGSECheck_MaxTime_Attr);
                    }
                    if (!m_vn->hasAttr(L"multiplier"))
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]GSE元素下MaxTime元素缺少multiplier属性", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                        SCDGSECheck(msg, bMaxTime_Attr, etSCDGSECheck_MaxTime_Attr);
                    }

                    cbAddrCache.MAXTime = VtdGetTextValue(m_vn);
                    m_vn->toElement(PARENT);
                }
                //else
                //{
                //    string msg = "GSE下缺少MaxTime子元素";
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
                // 判断子项是否被选中
                bool bAttr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_Attr").bEnabled;
                ErrorType etSCDPhysConnCheck_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_Attr").etValue;

                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_Child").bEnabled;
                ErrorType etSCDPhysConnCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_Child").etValue;

                bool bP_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_P_Attr").bEnabled;
                ErrorType etSCDPhysConnCheck_P_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDPhysConnCheck_P_Attr").etValue;

                bool bParentChecked = CSCDGSECheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr, 1); // PhysConn元素含有type属性
                AddDueElementInfo(bParentChecked, bChild, 1); // PhysConn元素含有一个必选参数P
                
                
                if (!m_vn->hasAttr(L"type"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s]PhysConn元素缺少type属性", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                    SCDPhysConnCheck(msg, bAttr, etSCDPhysConnCheck_Attr);
                }

                PhysConnCache pcCache;
                pcCache.PhysConnType = VtdGetAttributeValue(m_vn, L"type");
                pcCache.offset = GetOffsetOfVTD();

                if (m_vn->toElement(FIRST_CHILD, L"P"))
                {
                    AddDueElementInfo(bParentChecked, bP_Attr, 2); // P_type应含有Type和Plug 等2种类型

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
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s]PhysConn缺少type为Type的P元素", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                        SCDPhysConnCheck(msg, bP_Attr, etSCDPhysConnCheck_P_Attr);
                    }
                    if (!bHavePlug)
                    {
                        string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s]PhysConn缺少type为Plug的P元素", 
                            WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                        SCDPhysConnCheck(msg, bP_Attr, etSCDPhysConnCheck_P_Attr);
                    }

                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s]PhysConn元素缺少P子元素", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str());
                    SCDPhysConnCheck(msg, bChild, etSCDPhysConnCheck_Child);
                }

                PhysConnMap::iterator it = apCache.mapPhysConn.find(pcCache.Port);
                if (it != apCache.mapPhysConn.end())
                {
                    string msg = "ConnectedAP下PhysConn存在重复:SubnetWork name=%s ConnectedAP iedName=%s apName=%s PhysConn type=%s Port=%s";
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
                // 判断子项是否被选中
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
                AddDueElementInfo(bParentChecked, bAttr_R, 2); // SMV元素含有desc, IdInst及cbName等3个属性
                AddDueElementInfo(bParentChecked, bAttr_O, 1); // SMV元素含有desc, IdInst及cbName等3个属性
                // AddDueElementInfo(bParentChecked, bChild, 3); // SMV元素应含有子元素Address、MinTime、MaxTime 均为可选。 
                //AddDueElementInfo(bParentChecked, bMaxTime_Attr, 2); // MinTime元素含有unit和multiplier等2个属性
                //AddDueElementInfo(bParentChecked, bMinTime_Attr, 2); // MaxTime元素含有unit和multiplier等2个属性

                

                ControlBlockAddressCache cbAddrCache;
                cbAddrCache.ldInst = VtdGetAttributeValue(m_vn, L"ldInst");
                cbAddrCache.cbName = VtdGetAttributeValue(m_vn, L"cbName");
                cbAddrCache.key = cbAddrCache.ldInst + L"." + cbAddrCache.cbName;

                if (!m_vn->hasAttr(L"ldInst"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,SMV ldInst=%s cbName=%s]SMV元素下缺少ldInst属性", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDSMVCheck(msg, bAttr_R, etSCDSMVCheck_Attr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,SMV ldInst=%s cbName=%s]SMV元素下缺少desc属性", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDSMVCheck(msg, bAttr_O, etSCDSMVCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"cbName"))
                {
                    string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,SMV ldInst=%s cbName=%s]SMV元素下缺少cbName属性", 
                        WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.cbName).c_str());
                    SCDSMVCheck(msg, bAttr_R, etSCDSMVCheck_Attr_R);
                }

                int offset = GetOffsetOfVTD();
                cbAddrCache.offset = offset;

                if (m_vn->toElement(FIRST_CHILD, L"Address"))
                {
                    AddDueElementInfo(bParentChecked, bAddress_Child, 1); // Address元素是否含有一个必选参数P
                    AddDueElementInfo(bParentChecked, bP_Attr, 4); // P_type应含有MAC_Address, APPID, VLAN_PRIORITY和VLAN_ID等4种类型

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
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Address缺少MAC-Address信息", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDSMVCheck(msg, bP_Attr, etSCDSMVCheck_P_Attr);
                        }
                        if (!bHaveAppID)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Address缺少APPID信息", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDSMVCheck(msg, bP_Attr, etSCDSMVCheck_P_Attr);
                        }
                        if (!bHaveVLanID)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Address缺少VLanID信息", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDSMVCheck(msg, bP_Attr, etSCDSMVCheck_P_Attr);
                        }
                        if (!bHaveVLanPriority)
                        {
                            string msg = FormatXMLMessage("[SubNetWork name=%s, ConnectedAP iedName=%s apName=%s ,GSE ldInst=%s cbName=%s]Address缺少VLanPriority信息", 
                                WtoA(apCache.subnetName).c_str(), WtoA(apCache.iedName).c_str(), WtoA(apCache.apName).c_str(), WtoA(cbAddrCache.ldInst).c_str(), WtoA(cbAddrCache.ldInst).c_str());
                            SCDSMVCheck(msg, bP_Attr, etSCDSMVCheck_P_Attr);
                        }                        
                    }
                    //else
                    //{
                    //    string msg = "SMV的Address子元素缺少P子元素";
                    //    SCDSMVCheck(msg, bChild);
                    //}
                    
                    m_vn->toElement(PARENT);
                }
                //else
                //{
                //    string msg = "SMV下缺少Address子元素";
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
    // 实例化配置DOI/SDI/DAI的对象索引是否与数据类型模板定义的数据对象结构一致
    // 存在未被引用的LNType、DOtype、DAtype
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
                // 判断子项是否被选中
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
                AddDueElementInfo(bParentChecked, bAttr_R, 2); // LNodeType元素含有id,InClass等2个属性。
                AddDueElementInfo(bParentChecked, bAttr_O, 2); // LNodeType元素含有desc, ideType等2个属性。
                AddDueElementInfo(bParentChecked, bChild, 1); // LNodeType元素含有子元素DO数据对象元素。
                
                string id = WtoA(VtdGetAttributeValue(m_vn, L"id"));
                if (!m_vn->hasAttr(L"id"))
                {
                    string msg = FormatXMLMessage("[LNodeType id=%s]LNodeType元素缺少id属性", id.c_str());
                    SCDLNodeTypeCheck(msg, bAttr_R, etSCDLNodeTypeCheck_Attr_R);
                }
                if (!m_vn->hasAttr(L"desc"))
                {
                    string msg = FormatXMLMessage("[LNodeType id=%s]LNodeType元素缺少desc属性", id.c_str());
                    SCDLNodeTypeCheck(msg, bAttr_O, etSCDLNodeTypeCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"iedType"))
                {
                    string msg = FormatXMLMessage("[LNodeType id=%s]LNodeType元素缺少iedType属性", id.c_str());
                    SCDLNodeTypeCheck(msg, bAttr_O, etSCDLNodeTypeCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"lnClass"))
                {
                    string msg = FormatXMLMessage("[LNodeType id=%s]LNodeType元素缺少lnClass属性", id.c_str());
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
                        AddDueElementInfo(bParentChecked, bDO_Attr_R, 2); // DO元素含有name, type 2个属性。
                        AddDueElementInfo(bParentChecked, bDO_Attr_O, 2); // DO元素含有accessControl，transient等2个属性。

                        DOCache doName;
                        doName.offset = GetOffsetOfVTD();
                        doName.name = VtdGetAttributeValue(m_vn, L"name");
                        doName.type = VtdGetAttributeValue(m_vn, L"type");
                        doName.bUnUsed = true;

                        if (!m_vn->hasAttr(L"name"))
                        {
                            string msg = FormatXMLMessage("[LNodeType id=%s DO name=%s]DO缺少name属性", id.c_str(), WtoA(doName.name).c_str());
                            SCDLNodeTypeCheck(msg, bDO_Attr_R, etSCDLNodeTypeCheck_DO_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"type"))
                        {
                            string msg = FormatXMLMessage("[LNodeType id=%s DO name=%s]DO缺少type属性", id.c_str(), WtoA(doName.name).c_str());
                            SCDLNodeTypeCheck(msg, bDO_Attr_R, etSCDLNodeTypeCheck_DO_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"accessControl"))
                        {
                            string msg = FormatXMLMessage("[LNodeType id=%s DO name=%s]DO缺少accessControl属性", id.c_str(), WtoA(doName.name).c_str());
                            SCDLNodeTypeCheck(msg, bDO_Attr_O, etSCDLNodeTypeCheck_DO_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"transient"))
                        {
                            string msg = FormatXMLMessage("[LNodeType id=%s DO name=%s]DO缺少transient属性", id.c_str(), WtoA(doName.name).c_str());
                            SCDLNodeTypeCheck(msg, bDO_Attr_O, etSCDLNodeTypeCheck_DO_Attr_O);
                        }

                        lNodeType.mapDO.insert(make_pair(doName.name, doName));
                    } while (m_vn->toElement(NEXT_SIBLING, L"DO"));
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[LNodeType id=%s]LNodeType元素缺少DO子元素", id.c_str());
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
                // 判断子项是否被选中
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
                AddDueElementInfo(bParentChecked, bAttr_R, 2); // DOType元素含有id, cdc等2个属性
                AddDueElementInfo(bParentChecked, bAttr_O, 1); // DOType元素含有iedType及等1个属性
                AddDueElementInfo(bParentChecked, bChild, 2); // DOType元素含有子元素：SDO子数据对象元素、DA数据属性元素。均为必选元素
                              

                DOTypeCache doTypeCache;
                doTypeCache.offset = GetOffsetOfVTD();
                doTypeCache.id = VtdGetAttributeValue(m_vn, L"id");
                doTypeCache.cdc = VtdGetAttributeValue(m_vn, L"cdc");
                doTypeCache.bUnUsed = true;

                if (!m_vn->hasAttr(L"id"))
                {
                    string msg = FormatXMLMessage("[DOType id=%s]DOType元素缺少id属性", WtoA(doTypeCache.id).c_str());
                    SCDDOTypeCheck(msg, bAttr_R, etSCDDOTypeCheck_Attr_R);
                }
                if (!m_vn->hasAttr(L"iedType"))
                {
                    string msg = FormatXMLMessage("[DOType id=%s]DOType元素缺少iedType属性", WtoA(doTypeCache.id).c_str());
                    SCDDOTypeCheck(msg, bAttr_O, etSCDDOTypeCheck_Attr_O);
                }
                if (!m_vn->hasAttr(L"cdc"))
                {
                    string msg = FormatXMLMessage("[DOType id=%s]DOType元素缺少cdc属性", WtoA(doTypeCache.id).c_str());
                    SCDDOTypeCheck(msg, bAttr_R, etSCDDOTypeCheck_Attr_R);
                }

                // Get LNodeType Do
                if (m_vn->toElement(FIRST_CHILD, L"DA"))
                {
                    do 
                    {
                        AddDueElementInfo(bParentChecked, bDA_Attr_R, 3); // DA元素含有name, fc, bType 3个属性
                        AddDueElementInfo(bParentChecked, bDA_Attr_O, 1); // DA元素含有type 1个属性
                        

                        DACache daTypeCache;
                        daTypeCache.offset = GetOffsetOfVTD();
                        daTypeCache.name = VtdGetAttributeValue(m_vn, L"name");
                        daTypeCache.bType = VtdGetAttributeValue(m_vn, L"bType");
                        daTypeCache.type = VtdGetAttributeValue(m_vn, L"type");
                        daTypeCache.bUnUsed = true;

                        if (!m_vn->hasAttr(L"name"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s DA name=%s]DA元素缺少name属性", WtoA(doTypeCache.id).c_str(), WtoA(daTypeCache.name).c_str());
                            SCDDOTypeCheck(msg, bDA_Attr_R, etSCDDOTypeCheck_DA_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"fc"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s DA name=%s]DA元素缺少fc属性", WtoA(doTypeCache.id).c_str(), WtoA(daTypeCache.name).c_str());
                            SCDDOTypeCheck(msg, bDA_Attr_R, etSCDDOTypeCheck_DA_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"bType"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s DA name=%s]DA元素缺少bType属性", WtoA(doTypeCache.id).c_str(), WtoA(daTypeCache.name).c_str());
                            SCDDOTypeCheck(msg, bDA_Attr_R, etSCDDOTypeCheck_DA_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"type"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s DA name=%s]DA元素缺少type属性", WtoA(doTypeCache.id).c_str(), WtoA(daTypeCache.name).c_str());
                            SCDDOTypeCheck(msg, bDA_Attr_O, etSCDDOTypeCheck_DA_Attr_O);
                        }

                        doTypeCache.mapDaType.insert(make_pair(daTypeCache.name, daTypeCache));
                    } while (m_vn->toElement(NEXT_SIBLING, L"DA"));
                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[DOType id=%s]DOType元素缺少DA子元素", WtoA(doTypeCache.id).c_str());
                    SCDDOTypeCheck(msg, bChild, etSCDDOTypeCheck_Child_O);
                }
                // Get SDO Info
                if (m_vn->toElement(FIRST_CHILD, L"SDO"))
                {
                    do 
                    {
                        AddDueElementInfo(bParentChecked, bSDO_Attr_R, 2); // SDO元素含有name, type等2个属性
                        AddDueElementInfo(bParentChecked, bSDO_Attr_O, 1); // SDO元素含有desc等1个属性
                        

                        DOCache sDoCache;
                        sDoCache.offset = GetOffsetOfVTD();
                        sDoCache.name = VtdGetAttributeValue(m_vn, L"name");
                        sDoCache.type = VtdGetAttributeValue(m_vn, L"type");
                        sDoCache.bUnUsed = true;

                        if (!m_vn->hasAttr(L"name"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s SDO name=%s]SDO元素缺少name属性", WtoA(doTypeCache.id).c_str(), WtoA(sDoCache.name).c_str());
                            SCDDOTypeCheck(msg, bSDO_Attr_R, etSCDDOTypeCheck_SDO_Attr_R);
                        }
                        if (!m_vn->hasAttr(L"desc"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s SDO name=%s]SDO元素缺少desc属性", WtoA(doTypeCache.id).c_str(), WtoA(sDoCache.name).c_str());
                            SCDDOTypeCheck(msg, bSDO_Attr_O, etSCDDOTypeCheck_SDO_Attr_O);
                        }
                        if (!m_vn->hasAttr(L"type"))
                        {
                            string msg = FormatXMLMessage("[DOType id=%s SDO name=%s]SDO元素缺少type属性", WtoA(doTypeCache.id).c_str(), WtoA(sDoCache.name).c_str());
                            SCDDOTypeCheck(msg, bSDO_Attr_R, etSCDDOTypeCheck_SDO_Attr_R);
                        }

                        doTypeCache.mapSDO.insert(make_pair(sDoCache.name, sDoCache));
                    } while (m_vn->toElement(NEXT_SIBLING, L"SDO"));

                    m_vn->toElement(PARENT);
                }
                else
                {
                    string msg = FormatXMLMessage("[DOType id=%s]DOType元素缺少SDO子元素", WtoA(doTypeCache.id).c_str());
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
            // 判断子项是否被选中
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
            AddDueElementInfo(bParentChecked, bAttr_R, 1); // DAType元素含有id等1个属性
            AddDueElementInfo(bParentChecked, bAttr_O, 2); // DAType元素含有iedType及desc等2个属性
            AddDueElementInfo(bParentChecked, bChild, 1); // DAType元素含有必选子元素：BDA基础数据属性元素
                      
            DATypeCache daTypeCache;
            daTypeCache.offset = GetOffsetOfVTD();
            daTypeCache.id = VtdGetAttributeValue(m_vn, L"id");
            daTypeCache.bUnUsed = true;

            if (!m_vn->hasAttr(L"id"))
            {
                string msg = FormatXMLMessage("[DAType id=%s]DAType元素缺少id属性", WtoA(daTypeCache.id).c_str());
                SCDDATypeCheck(msg, bAttr_R, etSCDDATypeCheck_Attr_R);
            }
            if (!m_vn->hasAttr(L"iedType"))
            {
                string msg = FormatXMLMessage("[DAType id=%s]DAType元素缺少iedType属性", WtoA(daTypeCache.id).c_str());
                SCDDATypeCheck(msg, bAttr_O, etSCDDATypeCheck_Attr_O);
            }
            if (!m_vn->hasAttr(L"desc"))
            {
                string msg = FormatXMLMessage("[DAType id=%s]DAType元素缺少desc属性", WtoA(daTypeCache.id).c_str());
                SCDDATypeCheck(msg, bAttr_O, etSCDDATypeCheck_Attr_O);
            }

            // Get LNodeType Do
            if (m_vn->toElement(FIRST_CHILD, L"BDA"))
            {
                do 
                {
                    AddDueElementInfo(bParentChecked, bBDA_Attr_R, 2); // BDA元素含有name, bType等2个属性
                    AddDueElementInfo(bParentChecked, bBDA_Attr_O, 1); // BDA元素含有type等1个属性
                    

                    BDACache bdaCache;
                    bdaCache.offset = GetOffsetOfVTD();
                    bdaCache.name = VtdGetAttributeValue(m_vn, L"name");
                    bdaCache.bType = VtdGetAttributeValue(m_vn, L"bType");
                    bdaCache.type = VtdGetAttributeValue(m_vn, L"type");
                    bdaCache.bUnUsed = true;

                    if (!m_vn->hasAttr(L"name"))
                    {
                        string msg = FormatXMLMessage("[DAType id=%s BDA name=%s]BDA元素缺少name属性", WtoA(daTypeCache.id).c_str(), WtoA(bdaCache.name).c_str());
                        SCDDATypeCheck(msg, bBDA_Attr_R, etSCDDATypeCheck_BDA_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"bType"))
                    {
                        string msg = FormatXMLMessage("[DAType id=%s BDA name=%s]BDA元素缺少bType属性", WtoA(daTypeCache.id).c_str(), WtoA(bdaCache.name).c_str());
                        SCDDATypeCheck(msg, bBDA_Attr_R, etSCDDATypeCheck_BDA_Attr_R);
                    }
                    if (!m_vn->hasAttr(L"type"))
                    {
                        string msg = FormatXMLMessage("[DAType id=%s BDA name=%s]BDA元素缺少type属性", WtoA(daTypeCache.id).c_str(), WtoA(bdaCache.name).c_str());
                        SCDDATypeCheck(msg, bBDA_Attr_O, etSCDDATypeCheck_BDA_Attr_O);
                    }

                    daTypeCache.mapBDA.insert(make_pair(bdaCache.name, bdaCache));

                } while (m_vn->toElement(NEXT_SIBLING, L"BDA"));

                m_vn->toElement(PARENT);
            }
            else
            {
                string msg = FormatXMLMessage("[DAType id=%s]DAType元素缺少BDA子元素", WtoA(daTypeCache.id).c_str());
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
            // 判断子项是否被选中
            bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Attr_R").bEnabled;
            ErrorType etSCDEnumTypeCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Attr_R").etValue;

            bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Attr_O").bEnabled;
            ErrorType etSCDEnumTypeCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Attr_O").etValue;

            bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Child").bEnabled;
            ErrorType etSCDEnumTypeCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_Child").etValue;

            bool bEnumVal_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_EnumVal_Attr").bEnabled;
            ErrorType etSCDEnumTypeCheck_EnumVal_Attr = CConfigMgr::GetInstance()->GetSubItemConfig("SCDEnumTypeCheck_EnumVal_Attr").etValue;

            bool bParentChecked = CSCDEnumTypeCheck::GetInstance()->IsEnabled();
            AddDueElementInfo(bParentChecked, bAttr_R, 1); // EnumType元素含有id等1个属性
            AddDueElementInfo(bParentChecked, bAttr_O, 1); // EnumType元素含有desc等1个属性
            AddDueElementInfo(bParentChecked, bChild, 1); // EnumType元素含有必选子元素：EnumVal枚举值元素
                      


            EnumTypeCache enumTypeCache;
            enumTypeCache.offset = GetOffsetOfVTD();
            enumTypeCache.id = VtdGetAttributeValue(m_vn, L"id");
            enumTypeCache.bUnUsed = true;

            if (!m_vn->hasAttr(L"id"))
            {
                string msg = FormatXMLMessage("[EnumType id=%s]EnumType元素缺少id属性", WtoA(enumTypeCache.id).c_str());
                SCDEnumTypeCheck(msg, bAttr_R, etSCDEnumTypeCheck_Attr_R);
            }
            if (!m_vn->hasAttr(L"desc"))
            {
                string msg = FormatXMLMessage("[EnumType id=%s]EnumType元素缺少desc属性", WtoA(enumTypeCache.id).c_str());
                SCDEnumTypeCheck(msg, bAttr_O, etSCDEnumTypeCheck_Attr_O);
            }

            // Get LNodeType Do
            if (m_vn->toElement(FIRST_CHILD, L"EnumVal"))
            {
                do 
                {
                    AddDueElementInfo(bParentChecked, bEnumVal_Attr, 1); // EnumVal元素含有ord属性
                    if (!m_vn->hasAttr(L"ord"))
                    {
                        string msg = FormatXMLMessage("[EnumType id=%s]EnumVal元素缺少ord属性", WtoA(enumTypeCache.id).c_str());
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
                string msg = FormatXMLMessage("[EnumType id=%s]EnumType元素缺少EnumVal子元素", WtoA(enumTypeCache.id).c_str());
                SCDEnumTypeCheck(msg, bChild, etSCDEnumTypeCheck_Child);
            }


            m_SCDCache.sDTTCache.mapEnumType.insert(make_pair(enumTypeCache.id, enumTypeCache));
        } while (m_vn->toElement(NEXT_SIBLING, L"EnumType"));

        m_vn->toElement(PARENT);
    }
    return true;
}


//-------------------------------------------------------缓存内检测项--------------------------------------------------------------


// Communication下SMV的APPID的值是否为4位16进制值，是否在4000～7FFF取值范围内且不可为0
bool CCacheMgr::SmvAppidValidCheck(wstring &appid)
{
    if (CSmvAppidValidCheck::GetInstance()->IsEnabled())
    {
        if (appid < L"4000" || appid > L"7FFF" || appid.size() != 4)
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "Communication下SMV的APPID的值不是4位16进制值，且在4000到7FFF取值范围内: APPID value=%s";
            CSmvAppidValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(appid).c_str());
        }
    }
    return true;
}

// Communication下GSE的APPID的值是否为4位16进制值，是否在0000到3FFF取值范围内
bool CCacheMgr::GseAppidValidCheck(wstring &appid)
{
    if (CGseAppidValidCheck::GetInstance()->IsEnabled())
    {
        if (appid < L"0000" || appid > L"3FFF" || appid.size()!=4)
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "Communication下GSE的APPID的值不是4位16进制值，且在0000到3FFF取值范围内:APPID value=%s";
            CGseAppidValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(appid).c_str());
        }
    }
    return true;
}

// Communication下SMV和GSE的VLAN-ID是否为3位16进制值
bool CCacheMgr::VlanIdValidCheck(wstring &vlanID)
{
    if (CVlanValidCheck::GetInstance()->IsEnabled())
    {
        if (vlanID < L"000" || vlanID > L"FFF" || vlanID.size() != 3)
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "Communication下SMV和GSE的VLAN-ID不是3位16进制值:VLan-ID value=%s";
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
            string msg = "Communication下SMV和GSE的VLAN-PRIORITY不在0~7内:VLAN-PRIORITY value=%s";
            CVlanValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(vlanPoy).c_str());
        }
    }
    return true;
}

// GSE中MAC地址在01-0C-CD-01-00-00~01-0C-CD-01-FF-FF范围内
bool CCacheMgr::GseMacValidCheck(wstring &mac)
{
    if (CGseMacValidCheck::GetInstance()->IsEnabled())
    {
        if (mac.size() == 0 || mac < L"01-0C-CD-01-00-00" || mac > L"01-0C-CD-01-FF-FF")
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "GSE中MAC地址不在01-0C-CD-01-00-00~01-0C-CD-01-FF-FF范围内: MAC-Address value=%s";
            CGseMacValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(mac).c_str());
        }
    }
    return true;
}

// SMV中MAC地址在01-0C-CD-04-00-00~01-0C-CD-04-FF-FF范围内
bool CCacheMgr::SmvMacValidCheck(wstring &mac)
{
    if (CSmvMacValidCheck::GetInstance()->IsEnabled())
    {
        if (mac < L"01-0C-CD-04-00-00" || mac > L"01-0C-CD-04-FF-FF")
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "SMV中MAC地址不在01-0C-CD-04-00-00~01-0C-CD-04-FF-FF范围内: MAC-Address value=%s";
            CSmvMacValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(mac).c_str());
        }
    }
    return true;
}

// IP地址、子网掩码的每一位在0~255范围内
bool CCacheMgr::IpValidCheck(IPCache &ip)
{
    if (CIpValidCheck::GetInstance()->IsEnabled())
    {
        if (ip.IP.size() == 0 || !CIpValidCheck::GetInstance()->is_ipaddr(WtoA(ip.IP).c_str()))
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "IP地址取值不在0~255范围内:IP value=%s";
            CIpValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(ip.IP).c_str());
        }
        if (ip.IPSubNet.size() == 0 || !CIpValidCheck::GetInstance()->is_ipaddr(WtoA(ip.IPSubNet).c_str()))
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "子网掩码的取值范围不在0~255内: IP-SUBNET value=%s";
            CIpValidCheck::GetInstance()->Report(row, WtoA(xml), msg.c_str(),
                WtoA(ip.IPSubNet).c_str());
        }
    }
    return true;
}

// Communication中Goose SV通信配置重复检测
bool CCacheMgr::CommunicationGSEorSVUniqueCheck(ConnectedAPCache &apCache, ControlBlockAddressCache &cbAddrCache)
{
    if (CCommunicationGSEorSVUniqueCheck::GetInstance()->IsEnabled())
    {
        wstring key = apCache.key + L"." + cbAddrCache.key;
        if (setGooseOrSV.count(key) == 1)
        {
            string msg = "子网下GOOSE或SV存在重复:SubnetWork name=%s ConnectedAP iedName=%s apName=%s ldInst=%s cbName=%s";
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

// Communication下SMV和GSE中的APPID是否全局唯一
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
            string msg = "APPID不唯一:APPID value=%s";
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

// SCD文件中Communication下MAC-Address的值是否全局唯一
bool CCacheMgr::MacAddrUniqueCheck(wstring &mac)
{
    if (CMacAddrUniqueCheck::GetInstance()->IsEnabled())
    {
        // 若Mac为空，则报通信参数正确性错误
        if (mac.size() == 0)
        {
            return true;
        }
        if (setMac.count(mac) == 1)
        {
            wstring xml = GetXMLContent();
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string msg = "Mac地址存在重复:MAC-Address value=%s";
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

// SCD文件中IED的IP地址是否全局唯一
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
            string msg = "IP地址存在重复:IP value=%s";
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

// SCD文件中GSEControl的appID的值、SampledValueControl的smvID的值是否全局唯一ReportControl的rptID的值是否全局唯一。
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
            string msg = "控制块的appID或smvID不唯一: ID=%s";
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

// GOOSE 和SV配置的ExtRef内部intAddr的值在IED范围内是否唯一；
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
            string msg = "intAddr在IED内不唯一:IED name=%s LDevice inst=%s intAddr=%s";
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


// Communication下SubNetwork的子网命名是否符合规范中的命名要求；
bool CCacheMgr::SubFormatCheck(wstring &subNetWorkName)
{
    if (CSubFormatCheck::GetInstance()->IsEnabled())
    {
        if ((subNetWorkName.find(L"Subnetwork_Stationbus") == wstring::npos) &&
            (subNetWorkName.find(L"Subnetwork_Processbus") == wstring::npos))
        {
            string msg = "Communication下SubNetwork的子网命名不符合规范中的命名要求, \
                全站子网宜划分成站控层和过程层两个子网，命名分别为“Subnetwork_Stationbus”和“Subnetwork_Processbus”:SubNetwork name=%s";
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSubFormatCheck::GetInstance()->Report(row, xml, msg.c_str(),
                WtoA(subNetWorkName).c_str());
        }
    }
    return true;
}

// GOOSE 和SV配置的ExtRef内部intAddr的值是否符合规范中的命名格式要求，规范格式应为“LD/LN.DO.DA”(Q/GDW 396-2009)或“n-A:LD/LN.DO.DA”（Q/GDW 396-2012）；
bool CCacheMgr::InputsFormatCheck(LDCache &ldCache, wstring &intAddr)
{
    if (CInputsFormatCheck::GetInstance()->IsEnabled())
    {
        bool bFormatError = false;
        // 短地址为空
        if (intAddr.size() == 0)
        {
            bFormatError = true;
        }
        // 物理端口为空
        int pos = intAddr.find(L":");
        if (pos != wstring::npos && pos == 0)
        {
            bFormatError = true;
        }
        // 若只到LN也为空
        if (intAddr.find(L".") == wstring::npos)
        {
            bFormatError = true;
        }
        // 若包含非法字符
        string temp = WtoA(intAddr);
        for (int i = 0; i < temp.size(); i++)
        {
            char c = intAddr.at(i);
            // 字符非"A-Z"、"a-z"、"："、"/"、“-”、“_”；
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
            string msg = "ExtRef内部intAddr的值包含非法字符:IED name=%s LDevice inst=%s 内部地址intAddr=%s";
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

        int iSizeOfSubstation = 0; //记录Substation个数 
        int offset = GetOffsetOfVTD(); 
        string name; // 记录变电站名称

        if (m_vn->toElement(FIRST_CHILD, L"Substation"))
        {
                
            do 
            {
                iSizeOfSubstation++;
                if (iSizeOfSubstation > 1) // 防止数据被冲
                {
                    break;
                }

                int  iSubstationOffset = GetOffsetOfVTD();       
                // 判断子项是否被选中
                bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Attr_R").bEnabled;
                ErrorType etSSDSubstationCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Attr_R").etValue;

                bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Attr_O").bEnabled;
                ErrorType etSSDSubstationCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Attr_O").etValue;

                bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Child").bEnabled;
                ErrorType etSSDSubstationCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SSDSubstationCheck_Child").etValue;

                bool bParentChecked = CSSDSubstationCheck::GetInstance()->IsEnabled();
                AddDueElementInfo(bParentChecked, bAttr_R, 1); // 1个属性：name
                AddDueElementInfo(bParentChecked, bAttr_O, 4); // 4个属性：desc、dtype、area、areaDesc
                AddDueElementInfo(bParentChecked, bChild, 1); // 1个子元素：VoltageLevel

                name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
                // 属性获取及检测
                if (m_vn->hasAttr(L"name"))
                {
                    m_SCDCache.ssd.name = VtdGetAttributeValue(m_vn, L"name");
                }
                else
                {
                    string msg = FormatXMLMessage("[Substation name=%s]Substation元素缺少name属性", name.c_str());
                    SSDSubstationCheck(name, msg, bAttr_R, etSSDSubstationCheck_Attr_R);
                }
                if (m_vn->hasAttr(L"desc"))
                {
                    m_SCDCache.ssd.desc = VtdGetAttributeValue(m_vn, L"desc");
                }
                else
                {
                    string msg = FormatXMLMessage("[Substation name=%s]Substation元素缺少desc属性", name.c_str());
                    SSDSubstationCheck(name, msg, bAttr_O, etSSDSubstationCheck_Attr_O);
                }

                // 记录是否含有Voltagelevel、Private属性
                bool bHaveVoltageLevel = false;
                bool bHaveDType = false;
                bool bHaveAreaName = false;
                bool bHaveAreaDesc = false;

                // 遍历子元素
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
                            // SubStation是否含有其他元素（LNode等）
                            SSDSubstationCheck_OtherCheck(name, name, WtoA(tag));
                        }

                    } while (m_vn->toElement(NEXT_SIBLING));

                    // 返回父元素
                    m_vn->toElement(PARENT);
                }

                // 电压等级是否按照从高到低的顺序进行排序
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

                // 遍历完成子元素后判断是否含有VoltageLevel、dtype和area
                if (!bHaveVoltageLevel)
                {
                    string msg = FormatXMLMessage("[Substation name=%s]Substation元素缺少VoltageLevel子元素", name.c_str());
                    SSDSubstationCheck(name, msg, bChild, etSSDSubstationCheck_Child);
                }
                if (!bHaveDType)
                {
                    string msg = FormatXMLMessage("[Substation name=%s]Substation元素缺少dtype属性", name.c_str());
                    SSDSubstationCheck(name, msg, bAttr_O, etSSDSubstationCheck_Attr_O);
                }
                if (!bHaveAreaName)
                {
                    string msg = FormatXMLMessage("[Substation name=%s]Substation元素缺少area的name属性", name.c_str());
                    SSDSubstationCheck(name, msg, bAttr_O, etSSDSubstationCheck_Attr_O);
                }
                if (!bHaveAreaDesc)
                {
                    string msg = FormatXMLMessage("[Substation name=%s]Substation元素缺少area的desc属性", name.c_str());
                    SSDSubstationCheck(name, msg, bAttr_O, etSSDSubstationCheck_Attr_O);
                }

            } while (m_vn->toElement(NEXT_SIBLING, L"Substation"));
        
           
        }
        // 判断Substation是否唯一
        if (CSSDSubstationCheck_Unique::GetInstance()->IsEnabled())
        {
            int row = pChecker->CalcRowNumberFromOffset(offset);
            string xml = pChecker->GetRowContentByOffset(offset);
            if (iSizeOfSubstation > 1)
            {
                std::string message = "文件中存在多个Substation元素";
                CSSDSubstationCheck_Unique::GetInstance()->Report(name, CSSDSubstationCheck_Unique::GetInstance()->GetErrorType(), row, xml, message.c_str());
            }
            else if (iSizeOfSubstation == 0)
            {
                std::string message = "文件中不存在Substation元素";
                CSSDSubstationCheck_Unique::GetInstance()->Report(name, CSSDSubstationCheck_Unique::GetInstance()->GetErrorType(), row, xml, message.c_str());
            }
        }

        // 判断是否有ConnectivityNode未被使用
        SSDConnectivityNodeCheck_UnUsedCheck();
        // Terminal的SubstationName、VoltageLevelName、bayName、cNodeName、connectivityNode索引是否有效
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

        // 判断子项是否被选中
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
        AddDueElementInfo(bParentChecked, bAttr_R, 1); // 2个属性：name、desc
        AddDueElementInfo(bParentChecked, bChild_R, 1); // 2个子元素：Voltage、Bay
        AddDueElementInfo(bParentChecked, bAttr_O, 1); // 2个属性：name、desc
        AddDueElementInfo(bParentChecked, bChild_O, 1); // 2个子元素：Voltage、Bay
        
        string name = WtoA(VtdGetAttributeValue(m_vn, L"name"));
        string key = WtoA(substation.name) + "/" + name;
        voltagelevel.key = AtoW(key);
        if (m_vn->hasAttr(L"name"))
        {
            voltagelevel.name = VtdGetAttributeValue(m_vn, L"name");
        }
        else
        {
            string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevel缺少name属性", WtoA(substation.name).c_str(), name.c_str());
            SSDVoltageLevelCheck(key, msg, bAttr_R, etSSDVoltageLevelCheck_Attr_R);
        }
        if (m_vn->hasAttr(L"desc"))
        {
            voltagelevel.desc = VtdGetAttributeValue(m_vn, L"desc");
        } 
        else
        {
            string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevel缺少desc属性", WtoA(substation.name).c_str(), name.c_str());
            SSDVoltageLevelCheck(key, msg, bAttr_O, etSSDVoltageLevelCheck_Attr_O);
        }

        // VoltageLevel的name、desc属性是否符合额定电压标准
        SSDVoltageLevelCheck_NormativeCheck(key, WtoA(substation.name), WtoA(voltagelevel.name), true);
        SSDVoltageLevelCheck_NormativeCheck(key, WtoA(substation.name), WtoA(voltagelevel.desc), false);

        // 将信息存入Substation结构
        for (int j = 0; j < substation.vVoltageLevel.size(); j++)
        {
            if (substation.vVoltageLevel.at(j).name == voltagelevel.name)
            {
                string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevel不唯一", WtoA(substation.name).c_str(), name.c_str());
                SSDVoltageLevelCheck_UniqueCheck(key, msg);
            }
        }

        // 记录是否含有Bay、Voltage
        bool bHaveBay = false;
        bool bHaveVoltage = false;
        bool bHaveMultiplier = false;
        bool bHaveUnit = false;

        // 遍历子元素
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
                    AddDueElementInfo(bParentChecked, bVoltageAttr, 2); // Voltage 2个属性：name、desc

                    bHaveVoltage = true;

                    // 检测multiplier属性
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
                        string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevel缺少multiplier属性", WtoA(substation.name).c_str(), name.c_str());
                        SSDVoltageLevelCheck(key, msg, bVoltageAttr, etSSDVoltageLevelCheck_Voltage_Attr);
                    }

                    // 检测unit属性
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
                        string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevel缺少unit属性", WtoA(substation.name).c_str(), name.c_str());
                        SSDVoltageLevelCheck(key, msg, bVoltageAttr, etSSDVoltageLevelCheck_Voltage_Attr);
                    }

                    voltagelevel.voltage.text = VtdGetTextValue(m_vn);
                    // VoltageLevel为空/非空时，Voltage值是否为空非空
                    wstring voltage = voltagelevel.voltage.text + voltagelevel.voltage.multiplier + voltagelevel.voltage.unit;
                    if (!((voltagelevel.name == L"" && voltagelevel.voltage.text == L"") ||
                        (voltagelevel.name != L"" && voltage == voltagelevel.name)))
                    {
                        string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s Voltage text=%s]VoltageLevel值与Voltage不一致", WtoA(substation.name).c_str(), WtoA(voltagelevel.name).c_str(), WtoA(voltage).c_str());
                        SSDVoltageLevelCheck_UnNULLCheck(key, msg);
                    }
                }
                else
                {
                    // VoltageLevel是否含有其他元素（LNode等）
                    SSDVoltageLevelCheck_OtherCheck(key, WtoA(substation.name), name, WtoA(tag));
                }
            } while (m_vn->toElement(NEXT_SIBLING));

            // 返回父元素
            m_vn->toElement(PARENT);
        }

        if (!bHaveBay)
        {
            string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevel缺少Bay子元素", WtoA(substation.name).c_str(), name.c_str());
            SSDVoltageLevelCheck(key, msg, bChild_R, etSSDVoltageLevelCheck_Child_R);
        }
        if (!bHaveVoltage)
        {
            string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevel缺少Voltage子元素", WtoA(substation.name).c_str(), name.c_str());
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
        // 判断子项是否被选中
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Attr_R").bEnabled;
        ErrorType etSSDBayCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Attr_O").bEnabled;
        ErrorType etSSDBayCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Attr_O").etValue;

        bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Child").bEnabled;
        ErrorType etSSDBayCheck_Child = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Child").etValue;

        // bool bVirtual = CConfigMgr::GetInstance()->GetSubItemConfig("SSDBayCheck_Virtual").bEnabled;
        bool bParentChecked = CSSDBayCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 1); // 2个属性：name
        AddDueElementInfo(bParentChecked, bAttr_O, 2); // 2个属性：desc、dtype
        AddDueElementInfo(bParentChecked, bChild, 1); // 1个子元素：PowerTransformer或ConductingEquipment，ConnectivityNode， LNode
        // AddDueElementInfo(bParentChecked, bVirtual, 1); // Virtual 2个子元素：Function，LNode

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
            string msg = FormatXMLMessage("[VoltageLevel name=%s Bay name=%s]Bay元素缺少name属性", WtoA(voltageLevel.name).c_str(), bayName.c_str());
            SSDBayCheck(key, msg, bAttr_R, etSSDBayCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            bay.desc = VtdGetAttributeValue(m_vn, L"desc");
        }
        else
        {
            string msg = FormatXMLMessage("[VoltageLevel name=%s Bay name=%s]Bay元素缺少desc属性", WtoA(voltageLevel.name).c_str(), bayName.c_str());
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
                    wstring msg = L"含有非法子元素";
                }

            } while (m_vn->toElement(NEXT_SIBLING));

            // 返回父元素
            m_vn->toElement(PARENT);
        }

        // 一次设备间隔Bay元素的name、desc命名是否符合规范
        SSDBayCheck_AttrNormativeCheck(key, formatedMessage, bay);
        if (bay.ptMap.size() > 1)
        {
             SSDPowerTransformerCheck_UniqueCheck(key, formatedMessage);
        }       

        if (!bHaveDType)
        {
            string msg = FormatXMLMessage("[VoltageLevel name=%s Bay name=%s]Bay元素缺少dType属性", WtoA(voltageLevel.name).c_str(), bayName.c_str());
            SSDBayCheck(key, msg, bAttr_O, etSSDBayCheck_Attr_O);
        }
        if (!bHaveChild)
        {
            string msg = FormatXMLMessage("[VoltageLevel name=%s Bay name=%s]Bay元素缺少PowerTransformer、ConductingEquipment，ConnectivityNode，LNode中的任何一个", WtoA(voltageLevel.name).c_str(), bayName.c_str());
            SSDBayCheck(key, msg, bChild, etSSDBayCheck_Child);
        }
        //if (!bHaveVirtualChild)
        //{
        //    string msg = "虚拟Bay元素缺少Function，LNode中的任何一个";
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
        // 判断子项是否被选中
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Attr_R").bEnabled;
        ErrorType etSSDPowerTransformerCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Attr_O").bEnabled;
        ErrorType etSSDPowerTransformerCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Attr_O").etValue;

        bool bChild_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Child_R").bEnabled;
        ErrorType etSSDPowerTransformerCheck_Child_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Child_R").etValue;

        bool bChild_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Child_O").bEnabled;
        ErrorType etSSDPowerTransformerCheck_Child_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_Child_O").etValue;

        bool bParentChecked = CSSDPowerTransformerCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 2); // 4个属性：name、desc
        AddDueElementInfo(bParentChecked, bAttr_O, 2); // 4个属性：name、desc

        AddDueElementInfo(bParentChecked, bChild_R, 1); // 2个子元素：LNode和TransformerWinding
        AddDueElementInfo(bParentChecked, bChild_O, 1); // 2个子元素：LNode和TransformerWinding

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
            string msg = FormatXMLMessage("[%s, PowerTransformer name=%s]PowerTransformer元素缺少name属性", formatedPath.c_str(), ptName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bAttr_R, etSSDPowerTransformerCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            pt.desc = VtdGetAttributeValue(m_vn, L"desc");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, PowerTransformer name=%s]PowerTransformer元素缺少desc属性", formatedPath.c_str(), ptName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bAttr_O, etSSDPowerTransformerCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"virtual"))
        {
            pt.vir = VtdGetAttributeValue(m_vn, L"virtual");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, PowerTransformer name=%s]PowerTransformer元素缺少virtual属性", formatedPath.c_str(), ptName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bAttr_O, etSSDPowerTransformerCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"type"))
        {
            pt.type = VtdGetAttributeValue(m_vn, L"type");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, PowerTransformer name=%s]PowerTransformer元素缺少type属性", formatedPath.c_str(), ptName.c_str());
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

            // 返回父元素
            m_vn->toElement(PARENT);
        }
        
        if (pt.tfWindingMap.size() < 2)
        {
            SSDPowerTransformerCheck_WindingNormativeCheck(newKey, formatedMessage);
        }
        
        if (!bHaveTransformerWinding)
        {
            string msg = FormatXMLMessage("[%s]PowerTransformer元素缺少TransformerWinding子元素", formatedMessage.c_str());
            SSDPowerTransformerCheck(newKey, msg, bChild_R, etSSDPowerTransformerCheck_Child_R);
        }

        if (!bHaveLNode)
        {
            string msg = FormatXMLMessage("[%s]PowerTransformer元素缺少LNode子元素", formatedMessage.c_str());
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
        // 判断子项是否被选中
        bool bTfWindingAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_TFWinding_Attr_R").bEnabled;
        ErrorType etSSDPowerTransformerCheck_TFWinding_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_TFWinding_Attr_R").etValue;

        bool bTfWindingAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_TFWinding_Attr_O").bEnabled;
        ErrorType etSSDPowerTransformerCheck_TFWinding_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_TFWinding_Attr_O").etValue;

        // bool bTfWindingChild = CConfigMgr::GetInstance()->GetSubItemConfig("SSDPowerTransformerCheck_TFWinding_Child").bEnabled;
        bool bParentChecked = CSSDPowerTransformerCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bTfWindingAttr_R, 2); // 4个属性
        AddDueElementInfo(bParentChecked, bTfWindingAttr_O, 2); // 4个属性
        // AddDueElementInfo(bParentChecked, bTfWindingChild, 1); // 1个子元素：“LNode”,“Terminal”,“SubEquipment”,“TapChanger”。均为可选

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
            string msg = FormatXMLMessage("[%s, TransformerWinding name=%s]TransformerWinding元素缺少name属性", formatedPath.c_str(), transformWindingName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bTfWindingAttr_R, etSSDPowerTransformerCheck_TFWinding_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            tw.desc = VtdGetAttributeValue(m_vn, L"desc");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, TransformerWinding name=%s]TransformerWinding元素缺少desc属性", formatedPath.c_str(), transformWindingName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bTfWindingAttr_O, etSSDPowerTransformerCheck_TFWinding_Attr_O);
        }

        if (m_vn->hasAttr(L"virtual"))
        {
            tw.vir = VtdGetAttributeValue(m_vn, L"virtual");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, TransformerWinding name=%s]TransformerWinding元素缺少virtual属性", formatedPath.c_str(), transformWindingName.c_str());
            SSDPowerTransformerCheck(newKey, msg, bTfWindingAttr_O, etSSDPowerTransformerCheck_TFWinding_Attr_O);
        }

        if (m_vn->hasAttr(L"type"))
        {
            tw.type = VtdGetAttributeValue(m_vn, L"type");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, TransformerWinding name=%s]TransformerWinding元素缺少type属性", formatedPath.c_str(), transformWindingName.c_str());
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

            // 返回父元素
            m_vn->toElement(PARENT);
        }

        //if (!bHavaChild)
        //{
        //    string msg = "TransformerWinding元素缺少子元素";
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
        // 判断子项是否被选中
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConductingEquipmentCheck_Attr_R").bEnabled;
        ErrorType etSSDConductingEquipmentCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConductingEquipmentCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConductingEquipmentCheck_Attr_O").bEnabled;
        ErrorType etSSDConductingEquipmentCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConductingEquipmentCheck_Attr_O").etValue;
        // bool bChild = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConductingEquipmentCheck_Child").bEnabled;
        bool bParentChecked = CSSDConductingEquipmentCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 2); // 2个属性：name、desc
        AddDueElementInfo(bParentChecked, bAttr_O, 2); // 2个属性：name、desc
        // AddDueElementInfo(bParentChecked, bChild, 1); // 可选子元素

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
            string msg = FormatXMLMessage("[%s, ConductingEquipment name=%s]ConductingEquipment元素缺少name属性", formatedPath.c_str(), ceName.c_str());
            SSDConductingEquipmentCheck(newKey, msg, bAttr_R, etSSDConductingEquipmentCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            equipment.desc = VtdGetAttributeValue(m_vn, L"desc");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, ConductingEquipment name=%s]ConductingEquipment元素缺少desc属性", formatedPath.c_str(), ceName.c_str());
            SSDConductingEquipmentCheck(newKey, msg, bAttr_O, etSSDConductingEquipmentCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"virtual"))
        {
            equipment.vir = VtdGetAttributeValue(m_vn, L"virtual");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, ConductingEquipment name=%s]ConductingEquipment元素缺少virtual属性", formatedPath.c_str(), ceName.c_str());
            SSDConductingEquipmentCheck(newKey, msg, bAttr_O, etSSDConductingEquipmentCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"type"))
        {
            equipment.type = VtdGetAttributeValue(m_vn, L"type");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, ConductingEquipment name=%s]ConductingEquipment元素缺少type属性", formatedPath.c_str(), ceName.c_str());
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

            // 返回父元素
            m_vn->toElement(PARENT);
        }

        // 判断是否为引出线设备
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
        // 判断子项是否被选中
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDLNodeCheck_Attr_R").bEnabled;
        ErrorType etSSDLNodeCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDLNodeCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDLNodeCheck_Attr_O").bEnabled;
        ErrorType etSSDLNodeCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDLNodeCheck_Attr_O").etValue;

        bool bParentChecked = CSSDLNodeCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 4); // 4个属性：lnInst，lnClass，iedName，ldInst
        AddDueElementInfo(bParentChecked, bAttr_O, 2); // 2个属性：prefix及lnType。

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
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNode元素缺少iedName属性", formatedPath.c_str(), WtoA(key).c_str());
            SSDLNodeCheck(WtoA(key), msg, bAttr_R, etSSDLNodeCheck_Attr_R);
        }
        if (!m_vn->hasAttr(L"ldInst"))
        {
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNode元素缺少ldInst属性", formatedPath.c_str(), WtoA(key).c_str());
            SSDLNodeCheck(WtoA(key), msg, bAttr_R, etSSDLNodeCheck_Attr_R);
        }
        if (!m_vn->hasAttr(L"lnInst"))
        {
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNode元素缺少lnInst属性", formatedPath.c_str(), WtoA(key).c_str());
            SSDLNodeCheck(WtoA(key), msg, bAttr_R, etSSDLNodeCheck_Attr_R);
        }
        if (!m_vn->hasAttr(L"lnClass"))
        {
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNode元素缺少lnClass属性", formatedPath.c_str(), WtoA(key).c_str());
            SSDLNodeCheck(WtoA(key), msg, bAttr_R, etSSDLNodeCheck_Attr_R);
        }
        if (!m_vn->hasAttr(L"prefix"))
        {
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNode元素缺少prefix属性", formatedPath.c_str(), WtoA(key).c_str());
            SSDLNodeCheck(WtoA(key), msg, bAttr_O, etSSDLNodeCheck_Attr_O);
        }
        if (!m_vn->hasAttr(L"lnType"))
        {
            string msg = FormatXMLMessage("[%s, LNode path=%s]LNode元素缺少lnType属性", formatedPath.c_str(), WtoA(key).c_str());
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
        // 判断子项是否被选中
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDTerminalCheck_Attr_R").bEnabled;
        ErrorType etSSDTerminalCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDTerminalCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDTerminalCheck_Attr_O").bEnabled;
        ErrorType etSSDTerminalCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDTerminalCheck_Attr_O").etValue;

        bool bParentChecked = CSSDTerminalCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 6); // 7个属性：name、connectivityNode、substationName、voltageLevelName、 bayName和cnodeName等7个属性
        AddDueElementInfo(bParentChecked, bAttr_O, 1); // 7个属性：desc等7个属性
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
            string msg = FormatXMLMessage("[%s, Terminal name=%s]Terminal元素缺少name属性", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_R, etSSDTerminalCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            terminal.desc = VtdGetAttributeValue(m_vn, L"desc");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]Terminal元素缺少desc属性", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_O, etSSDTerminalCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"connectivityNode"))
        {
            terminal.connectivityNode = VtdGetAttributeValue(m_vn, L"connectivityNode");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]Terminal元素缺少connectivityNode属性", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_R, etSSDTerminalCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"substationName"))
        {
            terminal.substationName = VtdGetAttributeValue(m_vn, L"substationName");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]Terminal元素缺少substationName属性", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_R, etSSDTerminalCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"voltageLevelName"))
        {
            terminal.voltageLevelName = VtdGetAttributeValue(m_vn, L"voltageLevelName");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]Terminal元素缺少voltageLevelName属性", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_R, etSSDTerminalCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"bayName"))
        {
            terminal.bayName = VtdGetAttributeValue(m_vn, L"bayName");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]Terminal元素缺少bayName属性", formatedPath.c_str(), terminalName.c_str());
            SSDTerminalCheck(newKey, msg, bAttr_R, etSSDTerminalCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"cNodeName"))
        {
            terminal.cnodeName = VtdGetAttributeValue(m_vn, L"cNodeName");
        }
        else
        {
            string msg = FormatXMLMessage("[%s, Terminal name=%s]Terminal元素缺少cNodeName属性", formatedPath.c_str(), terminalName.c_str());
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
            // 同装置内重复
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
        // 判断子项是否被选中
        //bool bAttr = CConfigMgr::GetInstance()->GetSubItemConfig("SSDCommonEquipmentCheck_Attr").bEnabled;
        //bool bParentChecked = CSSDCommonEquipmentCheck::GetInstance()->IsEnabled();
        //AddDueElementInfo(bParentChecked, bAttr, 4); // 4个属性：name、desc、 virtual、type

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
        //    string msg = FormatXMLMessage("[%s, Equipment name=%s]设备缺少name属性", formatedPath.c_str(), comeqName.c_str());
        //    SSDCommonEquipmentCheck(newKey, msg, bAttr);
        //}

        //if (m_vn->hasAttr(L"desc"))
        //{
        //    equipment.desc = VtdGetAttributeValue(m_vn, L"desc");
        //} 
        //else
        //{
        //    string msg = FormatXMLMessage("[%s, Equipment name=%s]设备缺少desc属性", formatedPath.c_str(), comeqName.c_str());
        //    SSDCommonEquipmentCheck(newKey, msg, bAttr);
        //}

        //if (m_vn->hasAttr(L"virtual"))
        //{
        //    equipment.vir = VtdGetAttributeValue(m_vn, L"virtual");
        //} 
        //else
        //{
        //    string msg = FormatXMLMessage("[%s, Equipment name=%s]设备缺少virtual属性", formatedPath.c_str(), comeqName.c_str());
        //    SSDCommonEquipmentCheck(newKey, msg, bAttr);
        //}

        //if (m_vn->hasAttr(L"type"))
        //{
        //    equipment.type = VtdGetAttributeValue(m_vn, L"type");
        //} 
        //else
        //{
        //    string msg = FormatXMLMessage("[%s, Equipment name=%s]设备缺少type属性", formatedPath.c_str(), comeqName.c_str());
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
        // 判断子项是否被选中
        bool bAttr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConnectivityNodeCheck_Attr_R").bEnabled;
        ErrorType etSSDConnectivityNodeCheck_Attr_R = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConnectivityNodeCheck_Attr_R").etValue;

        bool bAttr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConnectivityNodeCheck_Attr_O").bEnabled;
        ErrorType etSSDConnectivityNodeCheck_Attr_O = CConfigMgr::GetInstance()->GetSubItemConfig("SSDConnectivityNodeCheck_Attr_O").etValue;

        bool bParentChecked = CSSDConnectivityNodeCheck::GetInstance()->IsEnabled();
        AddDueElementInfo(bParentChecked, bAttr_R, 2); // 3个属性：ConnectivityNode的name、pathName属性是否存在;
        AddDueElementInfo(bParentChecked, bAttr_O, 1); // 3个属性：ConnectivityNode的desc属性是否存在;
        
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
            string msg = FormatXMLMessage("[%s, ConnectivityNode name=%s]ConnectivityNode元素缺少name属性", formatedPath.c_str(), cnodeName.c_str());
            SSDConnectivityNodeCheck(newKey, msg, bAttr_R, etSSDConnectivityNodeCheck_Attr_R);
        }

        if (m_vn->hasAttr(L"desc"))
        {
            connectivityNode.desc = VtdGetAttributeValue(m_vn, L"desc");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, ConnectivityNode name=%s]ConnectivityNode元素缺少desc属性", formatedPath.c_str(), cnodeName.c_str());
            SSDConnectivityNodeCheck(newKey, msg, bAttr_O, etSSDConnectivityNodeCheck_Attr_O);
        }

        if (m_vn->hasAttr(L"pathName"))
        {
            connectivityNode.pathname = VtdGetAttributeValue(m_vn, L"pathName");
        } 
        else
        {
            string msg = FormatXMLMessage("[%s, ConnectivityNode name=%s]ConnectivityNode元素缺少pathName属性", formatedPath.c_str(), cnodeName.c_str());
            SSDConnectivityNodeCheck(newKey, msg, bAttr_R, etSSDConnectivityNodeCheck_Attr_R);
        }   

        // 唯一性 检测
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

// ---------------------------------------------------------------------------------------------------------完整性检测----------------------------------------------
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




// -----------------------------------------------------------------------------------SSD规范性检测------------------------------------------------------

bool CCacheMgr::SSDSubstationCheck_NormativeCheck(string key, string subStationName, string dType)
{
    if (CSSDSubstationCheck_Normative::GetInstance()->IsEnabled())
    {
        if (!CConfigMgr::GetInstance()->IsInSubstationTypeMap(dType))
        {
            string msg = FormatXMLMessage("[Substation name=%s]Substation变电站类型为%s,不符合标准中的任一类型", subStationName.c_str(), dType.c_str());
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
        string msg = FormatXMLMessage("[Substation name=%s]Substation含有未规定节点%s", subStationName.c_str(), tagName.c_str());
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
            string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]电压等级name不符合标准中的任一类型", subStationName.c_str(), vtgLevel.c_str());
            if (!bName)
            {
                msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]电压等级desc不符合标准中的任一类型", subStationName.c_str(), vtgLevel.c_str());
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
        string msg = FormatXMLMessage("[Substation name=%s VoltageLevel name=%s]VoltageLevel含有未规定节点%s", subStationName.c_str(), vtgLevel.c_str(), tagName.c_str());
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
        string msg = FormatXMLMessage("[Substation name=%s]VoltageLevel电压等级未按照从高到低的顺序进行排序", subStationName.c_str());
        int row = pChecker->CalcRowNumberFromOffset(offset);
        string xml = pChecker->GetRowContentByOffset(offset);
        CSSDVoltageLevelCheck_Sort::GetInstance()->Report(key, CSSDVoltageLevelCheck_Sort::GetInstance()->GetErrorType(), row, xml, msg.c_str());  
    }
    return true;
}

// 一次设备间隔Bay元素的name、desc命名是否符合规范
bool CCacheMgr::SSDBayCheck_AttrNormativeCheck(string key, string path, BayCache &bay)
{
    if (CSSDBayCheck_AttrNormative::GetInstance()->IsEnabled())
    {
        // 获取一次主设备
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
            // 若为找到 则选首装置
            if (eqName.size() == 0)
            {
                eqName = bay.ceMap.begin()->second.name;
                eqDesc = bay.ceMap.begin()->second.desc;
                eqType = bay.ceMap.begin()->second.type;
            }
        }

        // 判断间隔名称
        if (eqName != bay.name && eqName.size() > 0)
        {
            string msg = FormatXMLMessage("[%s]间隔的name采用所包含一次主设备的name, 其中一次主设备为%s", path.c_str(), WtoA(eqName).c_str());
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDBayCheck_AttrNormative::GetInstance()->Report(key, CSSDBayCheck_AttrNormative::GetInstance()->GetErrorType(), row, xml, msg.c_str());
        }
        // 判断间隔描述
        if ((eqDesc +  + L"间隔") != bay.desc && eqDesc.size() > 0)
        {
            string msg = FormatXMLMessage("[%s]间隔的desc采用所包含一次主元件的 中文名称+间隔 的方式命名,其中一次设备名称为%s", path.c_str(), WtoA(eqDesc).c_str());
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
            string msg = FormatXMLMessage("[VoltageLevel name=%s Bay name=%s]间隔的dType为%s,不符合规范", vtgLevel.c_str(), bayName.c_str(), dType.c_str());
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
        string msg = FormatXMLMessage("[%s]间隔内含有多个变压器设备，应只含有一个变压器", path.c_str());
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
            string msg = FormatXMLMessage("[%s]变压器设备name=%s应采用“类型代码”+“调度编号”描述", path.c_str(), ptName.c_str());
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
        string msg = FormatXMLMessage("[%s]变压器设备至少包含两个变压器绕组子元素", path.c_str());
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
            string msg = FormatXMLMessage("[%s]Terminal对象name=%s属性未按照Tn进行命名", path.c_str(), tmlName.c_str());
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
        string msg = FormatXMLMessage("[%s]Terminal对象在同一装置内不唯一", path.c_str());
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
            // 自我有效性检测
            wstring cnode = tml->substationName + L"/" + tml->voltageLevelName + L"/" + tml->bayName + L"/" + tml->cnodeName;
            if (cnode != tml->connectivityNode)
            {
                string msg = FormatXMLMessage("[%s]Terminal对象内connectivityNode与substationName、voltageLevelName、bayName及cNodeName所指对象[%s]不一致", WtoA(tml->key).c_str(), WtoA(cnode).c_str());
                int row = pChecker->CalcRowNumberFromOffset(tml->offset);
                string xml = pChecker->GetRowContentByOffset(tml->offset);
                CSSDTerminalCheck_Valid::GetInstance()->Report(WtoA(tml->key), CSSDTerminalCheck_Valid::GetInstance()->GetErrorType(), row, xml, msg.c_str());
            }
            else
            {
                // 一致情况下,与CNode进行比较
                std::map<std::wstring, ConnectivityNodeCache>::iterator itCNode = m_AllcNodeMap.find(tml->connectivityNode);
                if (itCNode == m_AllcNodeMap.end())
                {
                    string msg = FormatXMLMessage("[%s]Terminal对象内connectivityNode所指对象不存在", WtoA(tml->key).c_str());
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
        string msg = FormatXMLMessage("[%s]ConnectivityNode的pathName存在重复", path.c_str());
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
                string msg = FormatXMLMessage("[%s]ConnectivityNode未与Terminal关联", WtoA(itCNode->second.key).c_str());
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
            string msg = FormatXMLMessage("[%s]ConnectivityNode对象name=%s属性未按照Cn进行命名", path.c_str(), cNodeName.c_str());
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
        string msg = FormatXMLMessage("[%s]ConductingEquipment在同一间隔内存在重复", path.c_str());
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
            string msg = FormatXMLMessage("[%s]引出线设备前缀应为LN，后缀应为L/X, 当前设备名称为%s, 前缀为%s, 后缀为%s", WtoA(eq.path).c_str(), WtoA(eq.name).c_str(), WtoA(first).c_str(), WtoA(last).c_str());
            int row = pChecker->CalcRowNumberFromOffset(GetOffsetOfVTD());
            string xml = pChecker->GetRowContentByOffset(GetOffsetOfVTD());
            CSSDConductingEquipmentCheck_IFLEquipment::GetInstance()->Report(key, CSSDConductingEquipmentCheck_IFLEquipment::GetInstance()->GetErrorType(), row, xml, msg.c_str());
        }
    }
    return true;
}


