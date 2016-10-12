#ifndef _CACHEMGR_H
#define _CACHEMGR_H

#include "../ScdCheckCommon/Common.h"
#include "../ScdCheckCommon/VtdTools.h"
#include "ScdChecker.h"
#include <set>

// add by cxl
#include "..\ScdCheckCommon\ValidateCache.h"
// end

struct ServicesCache
{
    int offset;
    int dataset_offset;
    int dataset_max;
    int dataset_maxAttr;
    int report_offset;
    int report_max;
    int log_offset;
    int log_max;

    ServicesCache()
    {
        dataset_max = -1;
        dataset_maxAttr = -1;
        report_max = -1;
        log_max = -1;
    }
};

struct FcdaCache
{
    int offset;
    wstring sDataset;
    wstring sLdInst;
    wstring sLnClass;
    wstring sLnInst;
    wstring sDoName;
    wstring sDaName;
    wstring sFc;
    wstring sPrefix;
    bool bHasDoName;
    bool bHasDaName;

    FcdaCache()
    {
        bHasDaName = false;
        bHasDoName = false;
    }
};

struct DatasetCache
{
    wstring sName;
    wstring sAccessPoint;
    wstring sLDevice;
    int offset;
    vector<FcdaCache> vecFcda;
};

// ���ݼ����Ƶ����ƿ��ӳ��
typedef map<wstring,wstring> DSTOCB;
// ���ݼ����Ƶ��������ݽṹ��ӳ��
typedef map<wstring,DatasetCache> DATASETMAP;

typedef map<wstring,wstring> SignalPath;

struct LNCache
{
    int offset;
    wstring sLdInst;
    wstring sDesc;
    wstring sLnType;
    wstring sLnClass;
    wstring sInst;
    wstring sPrefix;
};
typedef map<wstring,LNCache> LNCacheMap;

struct LDeviceCache
{
    wstring sLdInst;
    wstring sAccessPoint;
    wstring sDesc;

    bool bLLN0; // �Ƿ���LLNO�ڵ�
    bool bLPHD; // �Ƿ���LPHD�ڵ�
    bool bHasGoose; // �Ƿ���Goose�ڵ�
    bool bHasSmv; // �Ƿ���Smv�ڵ�
    int offset;
    int nReportCB;// ������ƿ�����
    int nLogCB; // ��־���ƿ�����
    DSTOCB mapDsToCB;//���ݼ������ƿ����͵�ӳ��

    DATASETMAP mapDataset;
    SignalPath mapPath; // LN0/LN ���ܵ��ź�·��
    LNCacheMap mapLN;

    LDeviceCache()
    {
        bLLN0 = false;
        bLPHD = false;
        bHasGoose = false;
        bHasSmv = false;
        offset = 0;
        nReportCB = 0;
        nLogCB = 0;
    }
};

typedef map<wstring,LDeviceCache> LDeviceCacheMap;

struct ICDCache
{
    ServicesCache services;
    LDeviceCacheMap mapLDevice;
    int offset;
};

struct SdiCache
{
    enum Pos {IN_DO, IN_DA};

    Pos pos;
    DOMap mapSDO;
    DAMap mapDaType;
    BDAMap mapBDA;
};


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
    // �õ���׼LNodeType����
    LNodeTypeMap& CCacheMgr::GetLNodeTypeCacheOfStandard();
    // �õ�ICD LNodeType����
    LNodeTypeMap& CCacheMgr::GetLNodeTypeCacheOfICD();

    // ���ر�׼��DOType(Templates\DataTypes���°�396��.xml)
    DOTypeMap& GetDOTypeCacheOfStandard();
    // �õ�ICD DOType����
    DOTypeMap& GetDOTypeCacheOfICD();

    // ���ر�׼��DAType(Templates\AttributeTypes���°�396��.xml)
    DATypeMap& GetDATypeCacheOfStandard();
    // �õ�ICD DAType����
    DATypeMap& GetDATypeCacheOfICD();

    // ���ر�׼��EnumType
    EnumTypeMap& GetEnumTypeCacheOfStandard();
    // �õ�ICD EnumType����
    EnumTypeMap& GetEnumTypeCacheOfICD();

    DataSetTypeMap& GetDataSetTypeOfStandard();
    LDeviceTypeMap& GetLDeviceTypeOfStandard();
    ReportControlTypeMap& GetReportControlTypeOfStandard();
    IedTemplateMap& GetIedTemplateOfStandard();

    // �������ݽṹ������
public:
    // ��׼LNodeType����
    LNodeTypeMap m_lNodeTypeOfStandard;
    // ICD LNodeType����
    LNodeTypeMap m_lNodeTypeOfICD;
    // ��׼ DoType����
    DOTypeMap m_doTypeOfStandard;
    // ICD DoType����
    DOTypeMap m_doTypeOfICD;
    // ��׼ DAType����
    DATypeMap m_daTypeOfStandard;
    // ICD DAType����
    DATypeMap m_daTypeOfICD;
    // ��׼ EnumType����
    EnumTypeMap m_enumTypeOfStandard;
    // ICD EnumType����
    EnumTypeMap m_enumTypeOfIcd;
    // ��׼ DataSet����
    DataSetTypeMap m_datasetTypeOfStandard;
    // ��׼ LDevice����
    LDeviceTypeMap m_lDeviceTypeOfStandard;
    // ��׼ ReportControl����
    ReportControlTypeMap m_reportControlTypeOfStandard;
    // ��׼ IED ����
    IedTemplateMap m_iedTypeOfStandard;
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

private:
    LNCache m_curLN;// ��ǰ���ڱ�����LN
    DOICache m_curDoi;// ��ǰ���ڱ�����DOI
private:
    void TravellICD(VTDNav* pNav);
    void Travell_IED(VTDNav* pNav);
    void Travell_AccessPoint(VTDNav* pNav,ICDCache& icd);
    void Travell_Services(VTDNav* pNav, ServicesCache& services);
    void Travell_LN0(VTDNav* pNav, LDeviceCache& ldevice, ServicesCache& services);
    void Travell_DataSet(VTDNav* pNav, LDeviceCache& ldevice, int maxFcda);
    void Travell_FCDA(VTDNav* pNav, DatasetCache& dateset);
    void Travell_LN(VTDNav* pNav, LDeviceCache& ldevice);
    void Travell_Communication(VTDNav* pNav);
    void Travell_DOI(VTDNav* pNav, DOMap& mapDo);
    void Travell_SDI(VTDNav* pNav, DOICache& doi, SdiCache& sdi);
    void Travell_DAI(VTDNav* pNav, DOICache& doi, SdiCache& sdi);
};

#endif /* _CACHEMGR_H */