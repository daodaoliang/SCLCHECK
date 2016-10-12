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

// ��¼xmlԪ��˳�����������(name ��Ϊ��ʾ)
typedef std::vector<wstring> Order;

struct DOTypeCache
{
    bool bUnUsed; // Record the DoType is Used or not
    int offset;
    wstring id;
    wstring cdc;

    DOMap mapSDO;
    Order sdoOrder;//��¼sdoԪ�س��ֵ�˳��
    DAMap mapDaType;
    Order daOrder;//��¼daԪ�س��ֵ�˳��
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
    Order doOrder; // ��¼DO˳��
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

    LogicalNodeMap mapLogicalNode; // ���LN��Ϣ
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

typedef std::map<wstring, std::set<wstring>> VlinkByOutSignalMap;//SIGNAL_ID:����źţ�vector<_SIGNAL_CACHE_DATA>�������ź��б�
typedef std::map<wstring, wstring> VlinkByInSignalMap;//SIGNAL_ID:�����źţ�_SIGNAL_CACHE_DATA������ź�

typedef std::map<wstring, KeySet> IntervalMap;

typedef std::map<wstring, std::map<wstring, int>> AssoIEDMap;
// -----------------------------------------------------------------------------SSD�ṹ---------------------------------------------------------------
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

// �ӹ���
struct SubFunctionCache{
    wstring name;
    wstring desc;

    LNodeCacheMap lnMap;
    CommonEquipmentCacheMap generalEquipmentMap;
};

// ����
struct FunctionCache{
    wstring name;
    wstring desc;

    LNodeCacheMap lnMap;
    SubFunctionCache subFuncMap;
    CommonEquipmentCacheMap generalEquipmentMap;
};
typedef std::map<wstring, FunctionCache> FunctionCacheMap;

// �����豸
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

// ��ѹ������
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
// ��ѹ���豸
struct PowerTransformerCache{
    wstring key;
    wstring name;
    wstring desc;
    wstring vir;
    wstring type;

    TransformerWindingCacheMap tfWindingMap; // ����
    LNodeCacheMap lnodeMap; 
};
typedef std::map<wstring, PowerTransformerCache> PowerTransformerCacheMap;

struct BayCache{
    wstring key;
    wstring name;
    wstring desc;
    wstring dtype; // �������

    PowerTransformerCacheMap ptMap; //��ѹ���б�
    ConductingEquipmentCacheMap ceMap; //�����豸�б�
    ConnectivityNodeCacheMap cnMap; // ����ڵ㣻
    LNodeCacheMap lnMap; // LNode �б�
    FunctionCacheMap funcMap; // �������е�����
};
typedef std::map<wstring, BayCache> BayCacheMap;

struct VoltageCache{
    wstring multiplier;
    wstring unit;
    wstring text;
};

// ��ѹ�ȼ�
struct VoltageLevelCache{
    wstring key;
    wstring name;
    wstring desc;

    VoltageCache voltage;
    BayCacheMap bayMap;
};
typedef std::vector<VoltageLevelCache> VoltageLevelVector;

// Substation �ṹ
struct SubstationCache{
    wstring name;
    wstring desc;

    // private �ڵ�
    wstring dtype; // ���վ���� CIME-type
    wstring areaName; // ���վ������д CIME-area
    wstring areaDesc; // ���վ�������� CIME-area

    VoltageLevelVector vVoltageLevel; // �˴���vector�洢 ���ں��ڱȽ�˳��
};
// -----------------------------------------------------------------------------SSD�ṹ---------------------------------------------------------------

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

    // �����Լ����ӽṹ
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
// �� sLnClass+sInst
typedef map<wstring,LNTemplate> LNTemplateMap;

struct LDeviceTemplate
{
    wstring sInst;
    wstring sDesc;
    LNTemplateMap mapLN;
};
// �� sInst
typedef map<wstring,LDeviceTemplate> LDeviceTemplateMap;

struct IedTemplate
{
    wstring sVoltage;
    wstring sName;
    wstring sType;
    LDeviceTemplateMap mapLDevice;
};
// �� sName+sVoltage
typedef map<wstring,IedTemplate> IedTemplateMap;

vector<wstring> Split(const wstring& s,
    const wstring& match,
    bool removeEmpty = false,
    bool fullMatch = false);

