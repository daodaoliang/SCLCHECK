#ifndef _CACHEMGR_H
#define _CACHEMGR_H

#include "../ScdCheckCommon/Common.h"
#include "../ScdCheckCommon/VtdTools.h"
#include "../ScdCheckCommon/ValidateCache.h"
#include "ScdChecker.h"
#include <set>

class CCacheMgr : public CSingleInstance<CCacheMgr>, CNoCopy
{
public:
    CCacheMgr();

    virtual ~CCacheMgr();

public:
    // ������Ҫĳĳ���͵Ļ�����Ϣ
    void AddNeededCacheMark(const std::string &strMark);

    // �ж��Ƿ���Ҫĳĳ���͵Ļ�����Ϣ
    bool IsNeededCacheMarkExists(const std::string &strMark);

private:
    std::set<std::string> m_setPreDefinedMarks;

    // ����get����������
public:
    // SCD CRC
    bool GetSCDCRC();

    // IED Cache
    bool GetIEDCache(); // get IEDInfo
    bool CreatInterval(wstring &iedName);
    bool CreatAssoIEDMap(wstring &in, wstring &out, int offset);
    bool GetIEDCRC(IEDCache &iedCache);
    // bool GetAPCache(IEDCache &iedCache);
    bool GetLDCache(IEDCache &iedCache);
    bool GetDataSetCache(LDCache &ldCache);
    bool GetFCDACache(DataSetCache &dsCache);
    bool GetGSEControlCache(LDCache &ldCache);
    bool GetSampledValueControlCache(LDCache &ldCache);
    bool GetReportControlCache(LDCache &ldCache);
    bool GetLogControlCache( LDCache &ldCache );
    bool GetSCLControlCache(LDCache &ldCache); // �����Լ�����
    bool GetSettingControlCache(LDCache &ldCache); // �����Լ�����
    bool GetInputsCache(LDCache &ldCache);
    bool GetLogicNodeInstanceCache(LDCache &ldCache);
    bool GetDOICache(LDCache &ldCache);
    bool GetDAICache(LDCache &ldCache, LogicalNodesInstanceCache &lnInstanceCache);
    bool GetSDICache(LDCache &ldCache, LogicalNodesInstanceCache &lnInstanceCache);

    // Communication Cache
    bool GetCommunicationCache(); // get Communication info
    bool GetSubnetWorkCache();
    bool GetConnectedAPCache(SubNetWorkCache &subNetCache);
    bool GetIPCache(ConnectedAPCache &apCache);
    bool GetGSECache(ConnectedAPCache &apCache);
    bool GetSMVCache(ConnectedAPCache &apCache);
    bool GetPhysConnCache(ConnectedAPCache &apCache);
    // ���
    bool CommunicationGSEorSVUniqueCheck(ConnectedAPCache &apCache, ControlBlockAddressCache &cbAddrCache);
    bool AppidUniqueCheck(wstring &appid); // Communication��SMV��GSE�е�APPID�Ƿ�ȫ��Ψһ
    bool SmvAppidValidCheck(wstring &appid); // Communication��SMV��APPID��ֵ�Ƿ�Ϊ4λ16����ֵ���Ƿ���4000��7FFFȡֵ��Χ���Ҳ���Ϊ0
    bool GseAppidValidCheck(wstring &appid); // Communication��GSE��APPID��ֵ�Ƿ�Ϊ4λ16����ֵ���Ƿ���0000��3FFFȡֵ��Χ��
    bool VlanIdValidCheck(wstring &vlanID); // Communication��SMV��GSE��VLAN-ID�Ƿ�Ϊ3λ16����ֵ
    bool VlanPriorityValidCheck(wstring &vlanPoy); // VLAN-Priority�Ƿ���0~7��
    bool GseMacValidCheck(wstring &mac); // GSE��MAC��ַ��01-0C-CD-01-00-00~01-0C-CD-01-FF-FF��Χ��
    bool SmvMacValidCheck(wstring &mac); // SMV��MAC��ַ��01-0C-CD-04-00-00~01-0C-CD-04-FF-FF��Χ��
    bool IpValidCheck(IPCache &ip); // IP��ַ�����������ÿһλ��0~255��Χ��
    bool MacAddrUniqueCheck(wstring &mac); // SCD�ļ���Communication��GSE��MAC-Address��ֵ�Ƿ�ȫ��Ψһ
    bool IpUniqueCheck(wstring &ip); // SCD�ļ���IED��IP��ַ�Ƿ�ȫ��Ψһ
    bool ControlIdUniqueCheck(wstring &id); // SCD�ļ���GSEControl��appID��ֵ��SampledValueControl��smvID��ֵ�Ƿ�ȫ��ΨһReportControl��rptID��ֵ�Ƿ�ȫ��Ψһ��
    bool IntAddrUniqueCheck(LDCache &ldCache, wstring &intAddr);
    bool InputsFormatCheck(LDCache &ldCache, wstring &intAddr);
    bool SubFormatCheck(wstring &subNetWorkName); // Communication��SubNetwork�����������Ƿ���Ϲ淶�е�����Ҫ��

    // DataTypeTemplates Cache
    bool GetDataTypeTemplateCache(); 
    bool GetLNodeTypeCache();
    bool GetDOTypeCache();
    bool GetDATypeCache();
    bool GetEnumTypeCache();

    wstring GetXMLContent();
    long GetOffsetOfVTD();
    void ReportCheckResult(const wstring &message, const wstring &xml);

    // �������ݽṹ������
public:
    SCDCache m_SCDCache; // SCD cache
    KeySet m_PMDevice;

private:
    KeySet setAppID; // �����е�AppID
    KeySet setMac; // �����е�Mac
    KeySet setIP; // �����е�IP
    KeySet setCBId; // ���ƿ��е�id
    KeySet setIntAddr; // Inputs�е�intAddr

    KeySet setConnectedAP; // Communication AP 
    KeySet setGooseOrSV;

public:
    // Ԥ�������棬��ȥScdChecker��ȡȫ�ֵ�����������������������m_vn��
    // ����������ϣ�����������ʹ�ã���ΪNULL
    bool PreBuildCaches();

private:
    bool PreBuildCachesInternal();

private:
    // PreBuildCaches��������ʱ�õ��ĵ�����
    VTDNav *m_vn;
    CScdChecker *pChecker;


//------------------------ SSD SubStation---------------------------
public:    
    bool GetSSDSubstationCache();
    bool GetSSDVoltageLevelCache(SubstationCache &substation);
    bool GetSSDBayCache(VoltageLevelCache &voltageLevel);

    bool GetSSDPowerTransformerCache(string key, string &formatedPath, PowerTransformerCacheMap &ptMap);
    bool GetSSDTransformerWindingCache(string key, string &formatedPath, TransformerWindingCacheMap &twMap);

    bool GetSSDConductingEquipmentCache(string key, string &formatedPath, ConductingEquipmentCacheMap &eqipmentMap);
    bool GetSSDFunctionCache(string key, string &formatedPath, FunctionCacheMap &funcMap);

    bool GetSSDConnectivityNodeCache(string key, string &formatedPath, ConnectivityNodeCacheMap &connectivityNodeMap);
    bool GetSSDLNodeCache(string parentKey, string &formatedPath, LNodeCacheMap &lnMap);
    bool GetSSDCommonEquipmentCache(string key, string &formatedPath, CommonEquipmentCacheMap &ceMap);
    bool GetSSDTerminalCache(string key, string &formatedPath, TerminalCacheMap &terminalMap);

    // �����Ŀ
    bool SSDSubstationCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // SubStationԪ���Ƿ���Ϲ淶
    bool SSDVoltageLevelCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // VoltageLevelԪ���Ƿ���Ϲ淶��
    bool SSDBayCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // BayԪ���Ƿ���Ϲ淶��
    bool SSDPowerTransformerCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // ��ѹ��Ԫ��PowerTransformer�Ƿ���Ϲ淶
    bool SSDConductingEquipmentCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // �����豸ConductingEquipment�����Ƿ���ϱ�׼��
    bool SSDTerminalCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // �豸�˵�Terminal�����Ƿ���ϱ�׼
    bool SSDLNodeCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // LNodeԪ���Ƿ���Ϲ�����׼
    bool SSDConnectivityNodeCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // SSDConnectivityNodeCheckԪ���Ƿ���Ϲ�����׼
    bool SSDCommonEquipmentCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // SSDCommonEquipmentCheckԪ���Ƿ���Ϲ�����׼  ͨ���豸��TabChanger��SubEquipment

    // -----------------------------SSD �淶�Լ��-----------------------------------
    bool SSDSubstationCheck_NormativeCheck(string key, string subStationName, string dType);
    bool SSDSubstationCheck_OtherCheck(string key, string subStationName, string tagName);
    bool SSDVoltageLevelCheck_UniqueCheck(string key, string message);
    bool SSDVoltageLevelCheck_UnNULLCheck(string key, string message);
    bool SSDVoltageLevelCheck_NormativeCheck(string key, string subStationName, string vtgLevel, bool bName);
    bool SSDVoltageLevelCheck_OtherCheck(string key, string subStationName, string vtgLevel, string tagName);
    bool SSDVoltageLevelCheck_SortCheck(string key, string subStationName, int offset);
    
    bool SSDBayCheck_AttrNormativeCheck(string key, string path, BayCache &bay);
    bool SSDBayCheck_dTypeNormativeCheck(string key, string vtgLevel, string bayName, string dType);
    // bool SSDBayCheck_LineBayCheck();
    // bool SSDPowerTransformerCheck_ConnectionCheck();
    bool SSDPowerTransformerCheck_UniqueCheck(string key, string path);
    bool SSDPowerTransformerCheck_AttrNormativeCheck(string key, string path, string ptName);
    bool SSDPowerTransformerCheck_WindingNormativeCheck(string key, string path);

    bool SSDConductingEquipmentCheck_UniqueCheck(string key, string path);
    // bool SSDConductingEquipmentCheck_NormativeCheck();
    bool SSDConductingEquipmentCheck_IFLEquipmentCheck(string key, ConductingEquipmentCache &eq);

    bool SSDTerminalCheck_NameNormativeCheck(string key, string path, string tmlName);
    std::map<std::wstring, TerminalCache> m_AllTerminalMap;
    std::map<std::wstring, ConnectivityNodeCache> m_AllcNodeMap;
    // bool SSDTerminalCheck_CNodeConnectionCheck(string path, string cnodeName);
    bool SSDTerminalCheck_UniqueCheck(string key, string path, TerminalCache &terminal);
    bool SSDTerminalCheck_ValidCheck();

    bool SSDConnectivityNodeCheck_PathUniqueCheck(string key, string path, ConnectivityNodeCache& cNode);
    bool SSDConnectivityNodeCheck_UnUsedCheck();
    bool SSDConnectivityNodeCheck_NameNormativeCheck(string key, string path, string tmlName);

    //bool SSDLNodeCheck_ValidCheck();
    //bool SSDLNodeCheck_UniqueCheck();
    //bool SSDLNodeCheck_BeUsedCheck();
    //bool SSDLNodeCheck_InBayCheck();

// -----------------------------SCD Header-----------------------------------
    bool GetHeaderCache();
    bool GetHitemCache(HitemVector &vec);

    // �����Ŀ
    bool SCDHeaderCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDCommunicationCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDSubNetWorkCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDConnectedAPCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDIPAddressCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDGSECheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDSMVCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDPhysConnCheck(string message, bool bSubItemChecked, ErrorType etValue);

    // ---------------------------------DataTypeTemplate---------------
    bool SCDLNodeTypeCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDDOTypeCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDDATypeCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDEnumTypeCheck(string message, bool bSubItemChecked, ErrorType etValue);

    // ----------------------------------IED----------------------------
    bool GetServisesCache(IEDCache &iedCache);

    bool SCDIEDCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDIEDServicesCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDIEDAccessPointCheck(string message, bool bSubItemChecked, ErrorType etValue);
    bool SCDIEDLNCheck(string message, bool bSubItemChecked, ErrorType etValue);

private:
    float m_DueSCDElementInfo; // SCD�ļ�Ӧ������Ԫ����Ϣ
    float m_LoseSCDElementInfo; // SCD�ļ���ʧԪ������

    void AddDueElementInfo(bool bBigItemChecked, bool bSubChecked, int attrCount = 1);
    bool CheckTrgOpsAttr(bool bSubItemChecked, ErrorType etValue);
};

#endif /* _CACHEMGR_H */