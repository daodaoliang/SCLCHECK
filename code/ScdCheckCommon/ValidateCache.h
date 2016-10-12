#pragma once
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

// LogicalNodesInstaces Struct
struct LogicalNodesInstanceCache 
{
    int offset;
    wstring key;

    wstring LN;
    wstring LNType;
    wstring LNDesc;
    wstring DOI;
    wstring DOIDesc;
    wstring DAI;
    wstring dUVal;

    wstring CDC;
};

// LogicalNodesInstaces Map
typedef std::map<wstring, LogicalNodesInstanceCache> LogicalNodesInstancesMap;

// Record GSE or SMV info
struct ControlBlockAddressCache
{
    wstring key;
    int offset;

    wstring ldInst;
    wstring cbName;

    wstring MACAddress;
    wstring VLANID;
    wstring VLANPRIORITY;
    wstring APPID;
    wstring MAXTime;
    wstring MinTime;
};

// ControlBlockAddress Map
typedef std::map<wstring, ControlBlockAddressCache> ControlBlockAddressMap;

struct IPCache
{
    wstring IP;
    wstring IPSubNet;
};

struct PhysConnCache
{
    int offset;
    wstring PhysConnType;
    wstring Plug;
    wstring Port;
    wstring Type;
};

typedef std::map<wstring, PhysConnCache> PhysConnMap;

// Record IP info
struct ConnectedAPCache
{
    int offset;
    wstring key;
    wstring subnetName;
    wstring iedName;
    wstring apName;
    IPCache ip;
    PhysConnMap mapPhysConn;
    ControlBlockAddressMap mapGseCBAddress;
    ControlBlockAddressMap mapSmvCBAddress;
};
typedef std::map<wstring, ConnectedAPCache> ConnectedAPMap;

struct SubNetWorkCache 
{
    wstring subNetWorkName;
    ConnectedAPMap mapConnectedAPMap;
};

// CommunicationConnectConfiguration Map
typedef std::map<wstring, SubNetWorkCache> SubNetWorkMap;

struct DACache 
{
    bool bUnUsed; // Record the DoType is Used or not
    int offset;
    wstring name;
    wstring bType;
    wstring type;
    wstring dchg;
    wstring fc;
    wstring qchg;
    wstring eOption;
};

typedef std::map<wstring, DACache> DAMap;

struct DOCache 
{
    bool bUnUsed; // Record the SDO is Used or not
    int offset;
    wstring name;
    wstring type;
    wstring eOption;
};

typedef std::map<wstring, DOCache> DOMap;

// 记录xml元素顺序的数据类型(name 作为标示)
typedef std::vector<wstring> Order;

struct DOTypeCache
{
    bool bUnUsed; // Record the DoType is Used or not
    int offset;
    wstring id;
    wstring cdc;

    DOMap mapSDO;
    Order sdoOrder;//记录sdo元素出现的顺序
    DAMap mapDaType;
    Order daOrder;//记录da元素出现的顺序
};

typedef std::map<wstring, DOTypeCache> DOTypeMap;

struct BDACache 
{
    bool bUnUsed; // Record the DoType is Used or not
    int offset;
    wstring name;
    wstring bType;
    wstring type;
    wstring eOption;
};

typedef std::map<wstring, BDACache> BDAMap;

typedef std::map<wstring, wstring> EnumVal;

struct EnumTypeCache
{
    bool bUnUsed; // Record the DoType is Used or not
    int offset;
    wstring id;

    EnumVal mapEnumVal;
};

typedef std::map<wstring, EnumTypeCache> EnumTypeCacheMap;

struct DATypeCache
{
    bool bUnUsed; // Record the DoType is Used or not
    int offset;
    wstring id;

    BDAMap mapBDA;
    Order bdaOrder;
};

typedef std::map<wstring, DATypeCache> DATypeMap;

struct LNodeTypeCache 
{
    bool bUnUsed; // Record the LNodeType is Used or not
    int offset;

    wstring id;
    wstring lnClass;

    DOMap mapDO;
    Order doOrder; // 记录DO顺序
};

typedef std::map<wstring, LNodeTypeCache> LNodeTypeMap;

typedef std::set<wstring> KeySet;

struct ControlBlockInfoCache
{
    int offset;
    wstring name;
    wstring dataSetName; // dataSet name
    wstring appID; // appID or smvID
    KeySet connectedIEDs; // record Connected IEDs

    //ControlBlockAddressCache cbAddress;
};

typedef std::map<wstring, ControlBlockInfoCache> ControlBlockInfoMap;

struct InputsCache
{
    int offset;
    wstring outSignal;
    wstring inSignal;
    wstring Port;
    wstring intAddr;
    wstring iedName;
    wstring ldInst;
};
typedef std::map<wstring, InputsCache> InputsMap;

struct FCDACache
{
    wstring key;
    int offset;

    wstring ldInst;
    wstring lnClass;
    wstring prefix;
    wstring lnInst;
    wstring doName;
    wstring daName;
    wstring fc;
};

struct DOICache
{
    int offset;
    wstring sname;
    wstring sdesc;
};
// key is dataSet`s name, and the value is FCDA set
typedef std::map<wstring, FCDACache> FCDAMap;

struct DataSetCache
{
    wstring iedName;
    wstring ldInst;
    wstring name;
    FCDAMap mapFCDA;
};
typedef std::map<wstring, DataSetCache> DataSetMap;

struct LogicalNodeCache
{
    int offset;
    wstring key;
    wstring lnType;
};

typedef std::map<wstring, LogicalNodeCache> LogicalNodeMap;

struct LDCache 
{
    wstring key;

    wstring iedName;
    wstring apName;
    wstring inst;
    ControlBlockInfoMap mapSampledValueControl;
    ControlBlockInfoMap mapGSEControl;
    ControlBlockInfoMap mapReportControl;
    ControlBlockInfoMap mapLogControl;
    InputsMap mapInputs;
    DataSetMap mapDataSet;

    LogicalNodeMap mapLogicalNode; // 存放LN信息
    LogicalNodesInstancesMap mapSignalInstance;
};

typedef std::map<wstring, LDCache> LDMap;

struct Signal
{
    wstring DO;
    wstring DA;
    wstring desc;
    wstring CDC;
};

//struct APCache
//{
//    wstring name;
//    LDMap mapLD;
//};
//typedef std::map<wstring, APCache> APMap;

struct IEDCache 
{
    wstring name;
    wstring CRC;
    int offset;

    KeySet apSet;
    LDMap mapLD;
};

typedef std::map<wstring, IEDCache> IEDMap;

struct DataTypeTemplateCache 
{
    LNodeTypeMap mapLNode;
    DOTypeMap mapDoType;
    DATypeMap mapDaType;
    EnumTypeCacheMap mapEnumType;
};

typedef std::map<wstring, std::set<wstring>> VlinkByOutSignalMap;//SIGNAL_ID:输出信号，vector<_SIGNAL_CACHE_DATA>：输入信号列表
typedef std::map<wstring, wstring> VlinkByInSignalMap;//SIGNAL_ID:输入信号，_SIGNAL_CACHE_DATA：输出信号

typedef std::map<wstring, KeySet> IntervalMap;

typedef std::map<wstring, std::map<wstring, int>> AssoIEDMap;
// -----------------------------------------------------------------------------SSD结构---------------------------------------------------------------
struct TerminalCache{
    int offset;
    wstring key;

    wstring name;
    wstring desc;
    wstring connectivityNode;
    wstring substationName;
    wstring voltageLevelName;
    wstring bayName;
    wstring cnodeName;
};
typedef std::map<wstring, TerminalCache> TerminalCacheMap;

struct LNodeCache{
    int offset;
    wstring key;

    wstring lnInst;
    wstring lnClass;
    wstring iedName;
    wstring ldInst;
    wstring prefix;
    wstring lnType;
};
typedef std::map<wstring, LNodeCache> LNodeCacheMap;

struct ConnectivityNodeCache{
    int offset;
    bool bUsed;
    wstring key;
    wstring pathMessage;

    wstring name;
    wstring desc;
    wstring pathname;
};
typedef std::map<wstring, ConnectivityNodeCache> ConnectivityNodeCacheMap;

struct CommonEquipmentCache{
    wstring key;

    wstring name;
    wstring desc;
    wstring vir;
    wstring type;
};
typedef std::map<wstring, CommonEquipmentCache> CommonEquipmentCacheMap;

// 子功能
struct SubFunctionCache{
    wstring name;
    wstring desc;

    LNodeCacheMap lnMap;
    CommonEquipmentCacheMap generalEquipmentMap;
};

// 功能
struct FunctionCache{
    wstring name;
    wstring desc;

    LNodeCacheMap lnMap;
    SubFunctionCache subFuncMap;
    CommonEquipmentCacheMap generalEquipmentMap;
};
typedef std::map<wstring, FunctionCache> FunctionCacheMap;

// 导电设备
struct ConductingEquipmentCache{
    int offset;
    wstring key;
    wstring path;

    wstring name;
    wstring desc;
    wstring vir;
    wstring type;

    TerminalCacheMap terminalMap;
    LNodeCacheMap lnMap;
    CommonEquipmentCacheMap subEquipmentMap;
};
typedef std::map<wstring, ConductingEquipmentCache> ConductingEquipmentCacheMap;

// 变压器绕组
struct TransformerWindingCache{
    wstring key;
    wstring name;
    wstring desc;
    wstring vir;
    wstring type;

    TerminalCacheMap terminalMap;
    CommonEquipmentCacheMap subEquipmentMap;
    CommonEquipmentCacheMap tapChangerMap;
    LNodeCacheMap lnMap;
};
typedef std::map<wstring, TransformerWindingCache> TransformerWindingCacheMap;
// 变压器设备
struct PowerTransformerCache{
    wstring key;
    wstring name;
    wstring desc;
    wstring vir;
    wstring type;

    TransformerWindingCacheMap tfWindingMap; // 绕组
    LNodeCacheMap lnodeMap; 
};
typedef std::map<wstring, PowerTransformerCache> PowerTransformerCacheMap;

struct BayCache{
    wstring key;
    wstring name;
    wstring desc;
    wstring dtype; // 间隔类型

    PowerTransformerCacheMap ptMap; //变压器列表
    ConductingEquipmentCacheMap ceMap; //导电设备列表
    ConnectivityNodeCacheMap cnMap; // 导电节点；
    LNodeCacheMap lnMap; // LNode 列表
    FunctionCacheMap funcMap; // 虚拟间隔中的内容
};
typedef std::map<wstring, BayCache> BayCacheMap;

struct VoltageCache{
    wstring multiplier;
    wstring unit;
    wstring text;
};

// 电压等级
struct VoltageLevelCache{
    wstring key;
    wstring name;
    wstring desc;

    VoltageCache voltage;
    BayCacheMap bayMap;
};
typedef std::vector<VoltageLevelCache> VoltageLevelVector;

// Substation 结构
struct SubstationCache{
    wstring name;
    wstring desc;

    // private 节点
    wstring dtype; // 变电站类型 CIME-type
    wstring areaName; // 变电站区域缩写 CIME-area
    wstring areaDesc; // 变电站区域描述 CIME-area

    VoltageLevelVector vVoltageLevel; // 此处用vector存储 便于后期比较顺序
};
// -----------------------------------------------------------------------------SSD结构---------------------------------------------------------------

// ---------------------------------------Header------------------------
struct HitemCache
{
    wstring version;
    wstring reversion;
    wstring when;
    wstring who;
    wstring what;
    wstring why;
};

typedef std::vector<HitemCache> HitemVector;

struct HeaderCache
{
    wstring id;
    wstring version;
    wstring reversion;
    wstring toolID;
    wstring namestructure;

    HitemVector history;
};
// ---------------------------------------Header------------------------

struct SCDCache 
{
    wstring crc;
    SubNetWorkMap mapSubNet;
    IEDMap mapIED; // IEDS
    DataTypeTemplateCache sDTTCache;

    IntervalMap mapSwitchBay;
    IntervalMap mapAllBay;
    AssoIEDMap mapAssoIED;
    VlinkByOutSignalMap vlinkByOutSignal;
    VlinkByInSignalMap vlinkByInSignal;

    // 完整性检测添加结构
    SubstationCache ssd;
    HeaderCache     header;
};


struct EnumType
{
    wstring id;
    std::vector<wstring> vecVal;
    int offset;
};
typedef map<wstring,EnumType> EnumTypeMap;

struct DataSetType
{
    wstring sName;
    wstring sDesc;
};
typedef map<wstring,DataSetType> DataSetTypeMap;

struct ReportControlType
{
    wstring sName;
    wstring sDesc;
};
typedef map<wstring,ReportControlType> ReportControlTypeMap;

struct LDeviceType
{
    wstring sName;
    wstring sDesc;
};
typedef map<wstring,LDeviceType> LDeviceTypeMap;

struct LNTemplate
{
    wstring sOption;
    wstring sFuntype;
    wstring sDesc;
    wstring sInst;
    wstring sLnClass;
};
// 键 sLnClass+sInst
typedef map<wstring,LNTemplate> LNTemplateMap;

struct LDeviceTemplate
{
    wstring sInst;
    wstring sDesc;
    LNTemplateMap mapLN;
};
// 键 sInst
typedef map<wstring,LDeviceTemplate> LDeviceTemplateMap;

struct IedTemplate
{
    wstring sVoltage;
    wstring sName;
    wstring sType;
    LDeviceTemplateMap mapLDevice;
};
// 键 sName+sVoltage
typedef map<wstring,IedTemplate> IedTemplateMap;

vector<wstring> Split(const wstring& s,
    const wstring& match,
    bool removeEmpty = false,
    bool fullMatch = false);

