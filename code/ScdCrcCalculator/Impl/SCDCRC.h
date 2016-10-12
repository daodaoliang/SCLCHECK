#pragma once

#include <math.h>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <tchar.h>


#include <string>

#include "Markup.h"

#include "CRC.h"

using namespace std;

struct DataRef
{
	wstring sDAIorDOI;
	wstring sBType;
};
struct FCDA
{
	wstring sLdInst;
	wstring sPrefix;
	wstring sLnClass;
	wstring sLnInst;
	wstring sDoName;
	wstring sDaName;
	wstring sFc;
	wstring sBType;

	BOOL bPrefix;
    BOOL bDaName;
};
typedef std::vector<FCDA> FCDASeq;
typedef std::map<wstring, FCDASeq> DataSetMap;
typedef std::map<wstring, FCDA> FCDAMap;    
typedef std::map<wstring, wstring> PairMap; 
typedef std::map<wstring, DataRef> DataRefMap;
// 发送参数
struct OutCbRef
{
	wstring sCbType; // GSE、SMV
	wstring sPath;   // CB的路径
	wstring sAp;
	wstring sApPrivateString; // AP下的私有元素
	wstring sPhysConnString;//AP下的PhysConn元素
	wstring sPrivateInCB;//（含Private）
	wstring sCbCtrlString; // CB控制块元素参数
	wstring sCbString;
	bool bConnectedAp;
	wstring sDataSet;
	FCDAMap mapFcda;  // 键值LD/LN.DO.DA
	vector<wstring> vecFcdaKey;//按文件顺序存储键值

};

//Inputs中外部信号路径到（本端信号路径,设备）的映射
struct Input
{
	wstring sOutIED;//输出设备
	wstring sOutPath;//输出信号路径
	wstring sInAddr;//输入信号路径
};

typedef multimap<wstring,wstring> VirLinkOutIn;
typedef VirLinkOutIn::iterator VirLinkIt;


typedef map<wstring,Input> InputMap;

typedef std::map<wstring, PairMap> PairPairMap;  
typedef std::map<wstring, OutCbRef> OutCBMap;  // 键值为控制块的路径

struct IED
{
	wstring sDesc;          // IED的描述
	wstring sManufacturer;
	wstring sType;
	wstring sCrcInFile;

	OutCBMap mapOutcb;
	vector<wstring> vecCBKey;//按文件出现顺序放置的控制块键值

	map<wstring,wstring> mapSigCB;//信号路径到控制块路径的映射

	DataRefMap datadaiMap;  // 键值<LD/LN.DO.DA , DAI节点的字符串内容&bType> 针对GOOSE的，发送和接收的
	DataRefMap datadoiMap;  // 键值<LD/LN.DO>, DOI节点字符串内容>  针对SV的，只存接收的即可
	
	PairPairMap mapInput;  // 键值为对侧的装置名称 pairMap<sExPath， sInpath>
	
	//Inputs中外部信号路径到（本端信号路径,设备）的映射
	InputMap		mapInsignal;
	//输出到输入的一对多关联
	VirLinkOutIn mapOutIn;
	vector<wstring> vecInsignal;//按文件中出现顺序存储键值

	map<wstring,wstring> mapSignalDai;//信号路径到dai的映射

};
typedef std::map<wstring, IED> IEDMap; // 键值为IED的名称
////////////// 模板部分 //////////////////////////////////
struct wDO
{
	wstring sName;
	wstring sType;	
};
typedef std::map<wstring, wDO> DOMap;
typedef std::map<wstring, DOMap> LNodeTypeMap; // 键值为LNodeType的ID
// DOType
struct wDA
{
	wstring sName;
	wstring sbType;
	wstring sType;
};
typedef std::map<wstring, wDA> DAMap;
struct wSDO
{
	wstring sName;
	wstring sType;
};
typedef std::map<wstring, wSDO> SDOMap;
struct wDOType
{
	DAMap daMap;
	DOMap sdoMap;
};
typedef std::map<wstring, wDOType> DOTypeMap; // 键值为DOType的ID

struct wBDA
{
	wstring sName;
	wstring sbType;
	wstring sType;
};

typedef std::map<wstring, wBDA> BDAMap;
typedef std::map<wstring, BDAMap> DATypeMap;

struct wCrcCode
{
	wstring sIedName;
	wstring sIedDesc;
	wstring sIedType;
	wstring sIedManu;

	wstring sCrc;
	wstring sCrcInFile;

};
typedef std::vector<wCrcCode> CrcCodeVec;

struct TypeCache
{
    DOMap mapDo;
    DAMap mapDa;
    BDAMap mapBda;
    enum {IN_DO,IN_DA,IN_BDA} flage;
};

class SCDCRCCALCULATOR_API CSCDCRC
{
public:
	CSCDCRC(void);
	~CSCDCRC(void);
private:
    wstring m_sScd;
public:
	CCRC  m_objCrc;
	//PairMap m_LnClassmap;      // 记录实例中用到的bType类型对应的LnClass
	PairPairMap m_bTypeMap;      // 记录所有的bType类型
	IEDMap m_iedmap;             // IED
	IEDMap& m_IEDMap;             // 记录IED位置真实存在的IED

	CrcCodeVec  m_IedCrcCodeVec;    // 记录所有IED的校验码
	wstring     m_SCDCRCCode;          // 整个SCD文件的校验码
	
	LNodeTypeMap m_LNodeTypeMap;
	DOTypeMap    m_DOTypeMap;
	DATypeMap    m_DATypeMap;

	CMarkup m_xmlMarkUp; 

	// 加载SCD文件
	bool Load(const wchar_t* pwPath);
	bool Load(const char* pchPath);

	// 处理Communication
	bool CommImport();

	BOOL hasSignal(IED& ied,wstring sKey);
	// 处理IED
	bool IedImport();

	// 处理DataTypeTemplate
	bool DataTypeTemplateImport();

	// 写XMl
	bool WriteXml();

    void GetFcdaBType(vector<wstring> vecName, TypeCache& tc, wstring& sBType);
	// 获取DAI的路径
	bool GetDAIPath(wstring& sPath, wstring sDoType, DataRef& dateref);

    bool TravellDAI(IED& ied, wstring sPath);
    bool TravellSDI(IED& ied, wstring sPath);
    bool TravellDOI(IED& ied, wstring sPath);
};

