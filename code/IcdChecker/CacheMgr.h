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

// 数据集名称到控制块的映射
typedef map<wstring,wstring> DSTOCB;
// 数据集名称到具体数据结构的映射
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

    bool bLLN0; // 是否有LLNO节点
    bool bLPHD; // 是否有LPHD节点
    bool bHasGoose; // 是否有Goose节点
    bool bHasSmv; // 是否有Smv节点
    int offset;
    int nReportCB;// 报告控制块数量
    int nLogCB; // 日志控制块数量
    DSTOCB mapDsToCB;//数据集到控制块类型的映射

    DATASETMAP mapDataset;
    SignalPath mapPath; // LN0/LN 可能的信号路径
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
    // 声明需要某某类型的缓存信息
    void AddNeededCacheMark(const std::string &strMark);

    // 判断是否需要某某类型的缓存信息
    bool IsNeededCacheMarkExists(const std::string &strMark);

private:
    std::set<std::string> m_setPreDefinedMarks;

    // 缓存get函数定义区
public:
    // 得到标准LNodeType数据
    LNodeTypeMap& CCacheMgr::GetLNodeTypeCacheOfStandard();
    // 得到ICD LNodeType数据
    LNodeTypeMap& CCacheMgr::GetLNodeTypeCacheOfICD();

    // 加载标准的DOType(Templates\DataTypes（新版396）.xml)
    DOTypeMap& GetDOTypeCacheOfStandard();
    // 得到ICD DOType数据
    DOTypeMap& GetDOTypeCacheOfICD();

    // 加载标准的DAType(Templates\AttributeTypes（新版396）.xml)
    DATypeMap& GetDATypeCacheOfStandard();
    // 得到ICD DAType数据
    DATypeMap& GetDATypeCacheOfICD();

    // 加载标准的EnumType
    EnumTypeMap& GetEnumTypeCacheOfStandard();
    // 得到ICD EnumType数据
    EnumTypeMap& GetEnumTypeCacheOfICD();

    DataSetTypeMap& GetDataSetTypeOfStandard();
    LDeviceTypeMap& GetLDeviceTypeOfStandard();
    ReportControlTypeMap& GetReportControlTypeOfStandard();
    IedTemplateMap& GetIedTemplateOfStandard();

    // 缓存数据结构定义区
public:
    // 标准LNodeType数据
    LNodeTypeMap m_lNodeTypeOfStandard;
    // ICD LNodeType数据
    LNodeTypeMap m_lNodeTypeOfICD;
    // 标准 DoType数据
    DOTypeMap m_doTypeOfStandard;
    // ICD DoType数据
    DOTypeMap m_doTypeOfICD;
    // 标准 DAType数据
    DATypeMap m_daTypeOfStandard;
    // ICD DAType数据
    DATypeMap m_daTypeOfICD;
    // 标准 EnumType数据
    EnumTypeMap m_enumTypeOfStandard;
    // ICD EnumType数据
    EnumTypeMap m_enumTypeOfIcd;
    // 标准 DataSet数据
    DataSetTypeMap m_datasetTypeOfStandard;
    // 标准 LDevice数据
    LDeviceTypeMap m_lDeviceTypeOfStandard;
    // 标准 ReportControl数据
    ReportControlTypeMap m_reportControlTypeOfStandard;
    // 标准 IED 数据
    IedTemplateMap m_iedTypeOfStandard;
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

private:
    LNCache m_curLN;// 当前正在遍历的LN
    DOICache m_curDoi;// 当前正在遍历的DOI
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