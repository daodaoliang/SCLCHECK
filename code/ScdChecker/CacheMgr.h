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
    // 声明需要某某类型的缓存信息
    void AddNeededCacheMark(const std::string &strMark);

    // 判断是否需要某某类型的缓存信息
    bool IsNeededCacheMarkExists(const std::string &strMark);

private:
    std::set<std::string> m_setPreDefinedMarks;

    // 缓存get函数定义区
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
    bool GetSCLControlCache(LDCache &ldCache); // 完整性检测添加
    bool GetSettingControlCache(LDCache &ldCache); // 完整性检测添加
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
    // 检测
    bool CommunicationGSEorSVUniqueCheck(ConnectedAPCache &apCache, ControlBlockAddressCache &cbAddrCache);
    bool AppidUniqueCheck(wstring &appid); // Communication下SMV和GSE中的APPID是否全局唯一
    bool SmvAppidValidCheck(wstring &appid); // Communication下SMV的APPID的值是否为4位16进制值，是否在4000～7FFF取值范围内且不可为0
    bool GseAppidValidCheck(wstring &appid); // Communication下GSE的APPID的值是否为4位16进制值，是否在0000到3FFF取值范围内
    bool VlanIdValidCheck(wstring &vlanID); // Communication下SMV和GSE的VLAN-ID是否为3位16进制值
    bool VlanPriorityValidCheck(wstring &vlanPoy); // VLAN-Priority是否在0~7内
    bool GseMacValidCheck(wstring &mac); // GSE中MAC地址在01-0C-CD-01-00-00~01-0C-CD-01-FF-FF范围内
    bool SmvMacValidCheck(wstring &mac); // SMV中MAC地址在01-0C-CD-04-00-00~01-0C-CD-04-FF-FF范围内
    bool IpValidCheck(IPCache &ip); // IP地址、子网掩码的每一位在0~255范围内
    bool MacAddrUniqueCheck(wstring &mac); // SCD文件中Communication下GSE的MAC-Address的值是否全局唯一
    bool IpUniqueCheck(wstring &ip); // SCD文件中IED的IP地址是否全局唯一
    bool ControlIdUniqueCheck(wstring &id); // SCD文件中GSEControl的appID的值、SampledValueControl的smvID的值是否全局唯一ReportControl的rptID的值是否全局唯一。
    bool IntAddrUniqueCheck(LDCache &ldCache, wstring &intAddr);
    bool InputsFormatCheck(LDCache &ldCache, wstring &intAddr);
    bool SubFormatCheck(wstring &subNetWorkName); // Communication下SubNetwork的子网命名是否符合规范中的命名要求；

    // DataTypeTemplates Cache
    bool GetDataTypeTemplateCache(); 
    bool GetLNodeTypeCache();
    bool GetDOTypeCache();
    bool GetDATypeCache();
    bool GetEnumTypeCache();

    wstring GetXMLContent();
    long GetOffsetOfVTD();
    void ReportCheckResult(const wstring &message, const wstring &xml);

    // 缓存数据结构定义区
public:
    SCDCache m_SCDCache; // SCD cache
    KeySet m_PMDevice;

private:
    KeySet setAppID; // 子网中的AppID
    KeySet setMac; // 子网中的Mac
    KeySet setIP; // 子网中的IP
    KeySet setCBId; // 控制块中的id
    KeySet setIntAddr; // Inputs中的intAddr

    KeySet setConnectedAP; // Communication AP 
    KeySet setGooseOrSV;

public:
    // 预构建缓存，会去ScdChecker中取全局导航器，并将导航器保存在m_vn中
    // 函数运行完毕，导航器不再使用，置为NULL
    bool PreBuildCaches();

private:
    bool PreBuildCachesInternal();

private:
    // PreBuildCaches过程中临时用到的导航器
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

    // 检测条目
    bool SSDSubstationCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // SubStation元素是否符合规范
    bool SSDVoltageLevelCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // VoltageLevel元素是否符合规范；
    bool SSDBayCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // Bay元素是否符合规范；
    bool SSDPowerTransformerCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // 变压器元素PowerTransformer是否符合规范
    bool SSDConductingEquipmentCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // 导电设备ConductingEquipment命名是否符合标准；
    bool SSDTerminalCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // 设备端点Terminal对象是否符合标准
    bool SSDLNodeCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // LNode元素是否符合国网标准
    bool SSDConnectivityNodeCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // SSDConnectivityNodeCheck元素是否符合国网标准
    bool SSDCommonEquipmentCheck(string key, string message, bool bSubItemChecked, ErrorType etValue); // SSDCommonEquipmentCheck元素是否符合国网标准  通用设备如TabChanger、SubEquipment

    // -----------------------------SSD 规范性检测-----------------------------------
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

    // 检测条目
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
    float m_DueSCDElementInfo; // SCD文件应包含的元素信息
    float m_LoseSCDElementInfo; // SCD文件丢失元素数量

    void AddDueElementInfo(bool bBigItemChecked, bool bSubChecked, int attrCount = 1);
    bool CheckTrgOpsAttr(bool bSubItemChecked, ErrorType etValue);
};

#endif /* _CACHEMGR_H */