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

typedef std::map<wstring, wstring> AttributesMap;

struct DataRef
{
	wstring sDAIorDOI;
	wstring sBType;
};
struct FCDA
{
    AttributesMap mapAttributes;
	wstring sBType;

	BOOL bPrefix;
    BOOL bDaName;
};
typedef std::vector<FCDA> FCDASeq;
typedef std::map<wstring, FCDASeq> DataSetMap;
typedef std::map<wstring, FCDA> FCDAMap;    
typedef std::map<wstring, wstring> PairMap; 
typedef std::map<wstring, DataRef> DataRefMap;
// ���Ͳ���
struct OutCbRef
{
    AttributesMap mapConnectedAPAttributes;
	wstring sCbType; // GSE��SMV
	wstring sPath;   // CB��·��
	wstring sAp;
    wstring sApConnectedAPString; // AP�µ�˽��Ԫ��
	wstring sApPrivateString; // AP�µ�˽��Ԫ��
	wstring sPhysConnString;//AP�µ�PhysConnԪ��
	wstring sPrivateInCB;//����Private��
	wstring sCbCtrlString; // CB���ƿ�Ԫ�ز���
	wstring sCbString; // Ap��GSE/SMV��Ϣ
	bool bConnectedAp;
	wstring sDataSet;
	FCDAMap mapFcda;  // ��ֵLD/LN.DO.DA
	vector<wstring> vecFcdaKey;//���ļ�˳��洢��ֵ

};

//Inputs���ⲿ�ź�·�����������ź�·��,�豸����ӳ��
struct Input
{
	wstring sOutIED;//����豸
	wstring sOutPath;//����ź�·��
	wstring sInAddr;//�����ź�·��
};

typedef multimap<wstring,wstring> VirLinkOutIn;
typedef VirLinkOutIn::iterator VirLinkIt;


typedef map<wstring,Input> InputMap;

typedef std::map<wstring, PairMap> PairPairMap;  
typedef std::map<wstring, OutCbRef> OutCBMap;  // ��ֵΪ���ƿ��·��

struct IED
{
    AttributesMap mapAttributes;
	wstring sCrcInFile;

	OutCBMap mapOutcb;
	vector<wstring> vecCBKey;//���ļ�����˳����õĿ��ƿ��ֵ

	map<wstring,wstring> mapSigCB;//�ź�·�������ƿ�·����ӳ��

	DataRefMap datadaiMap;  // ��ֵ<LD/LN.DO.DA , DAI�ڵ���ַ�������&bType> ���GOOSE�ģ����ͺͽ��յ�
	DataRefMap datadoiMap;  // ��ֵ<LD/LN.DO>, DOI�ڵ��ַ�������>  ���SV�ģ�ֻ����յļ���
	
	PairPairMap mapInput;  // ��ֵΪ�Բ��װ������ pairMap<sExPath�� sInpath>
	
	//Inputs���ⲿ�ź�·�����������ź�·��,�豸����ӳ��
	InputMap		mapInsignal;
	//����������һ�Զ����
	VirLinkOutIn mapOutIn;
	vector<wstring> vecInsignal;//���ļ��г���˳��洢��ֵ

	map<wstring,wstring> mapSignalDai;//�ź�·����dai��ӳ��

    map<wstring, wstring> mapSignalDaiDesc; // ��?���D?o?��?DO?����?
};
typedef std::map<wstring, IED> IEDMap; // ��ֵΪIED������
////////////// ģ�岿�� //////////////////////////////////
struct wDO
{
	wstring sName;
	wstring sType;	
};
typedef std::map<wstring, wDO> DOMap;
typedef std::map<wstring, DOMap> LNodeTypeMap; // ��ֵΪLNodeType��ID
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
typedef std::map<wstring, wDOType> DOTypeMap; // ��ֵΪDOType��ID

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
typedef std::map<wstring, wCrcCode> CrcCodeMap;

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
	//PairMap m_LnClassmap;      // ��¼ʵ�����õ���bType���Ͷ�Ӧ��LnClass
	PairPairMap m_bTypeMap;      // ��¼���е�bType����
	IEDMap m_iedmap;             // IED
	IEDMap& m_IEDMap;             // ��¼IEDλ����ʵ���ڵ�IED

	CrcCodeVec  m_IedCrcCodeVec;    // ��¼����IED��У����
    CrcCodeMap  m_IedCrcCodeMap;    // ��¼����IED��У����
	wstring     m_SCDCRCCode;          // ����SCD�ļ���У����
	
	LNodeTypeMap m_LNodeTypeMap;
	DOTypeMap    m_DOTypeMap;
	DATypeMap    m_DATypeMap;

	CMarkup m_xmlMarkUp; 

	// ����SCD�ļ�
	bool Load(const wchar_t* pwPath);
	bool Load(const char* pchPath);

	// ����Communication
	bool CommImport();

	BOOL hasSignal(IED& ied,wstring sKey);
	// ����IED
	bool IedImport();

	// ����DataTypeTemplate
	bool DataTypeTemplateImport();

	// дXMl
	bool WriteXml();
    bool WriteCCD();

    void GetFcdaBType(vector<wstring> vecName, TypeCache& tc, wstring& sBType);
	// ��ȡDAI��·��
	bool GetDAIPath(wstring& sPath, wstring sDoType, DataRef& dateref);

    bool TravellDAI(IED& ied, wstring sPath, wstring &sDoDesc);
    bool TravellSDI(IED& ied, wstring sPath, wstring &sDoDesc);
    bool TravellDOI(IED& ied, wstring sPath);
};

