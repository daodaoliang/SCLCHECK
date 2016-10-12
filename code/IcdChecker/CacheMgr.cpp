#include "CacheMgr.h"
#include "ScdChecker.h"
#include "../ScdCheckCommon/charconv.h"
#include <string>

// add by cxl
#include <windows.h>
#include <Shlwapi.h>
#include <atlstr.h>
#include "OutputMgr.h"

#include "CheckItems/IedCheck.h"
#include "CheckItems/GsSmvCheck.h"
#include "CheckItems/ConfDataSetCheck.h"
#include "CheckItems/ConfReportCheck.h"
#include "CheckItems/ConfLogCheck.h"
#include "CheckItems/ServerCheck.h"
#include "CheckItems/RedInputsCheck.h"
#include "CheckItems/ConfFcdaCheck.h"
#include "CheckItems/PhsyconCheck.h"
#include "CheckItems/DataSetCheck.h"
#include "CheckItems/ReportCtrlCheck.h"
#include "CheckItems/LdValidCheck.h"
#include "CheckItems/LnValidCheck.h"
#include "CheckItems/LdDescCheck.h"
#include "CheckItems/LDdoiCheck.h"
#include "CheckItems/FcdaLdValidCheck.h"
#include "CheckItems/DataSetTypeValidCheck.h"
#include "CheckItems/FcdaValidCheck.h"
#include "CheckItems/DoiValidCheck.h"
#include "CheckItems/GeneralErrorCheck.h"
#include "CheckItems/DotypeCheck.h"
// end

CCacheMgr::CCacheMgr(void) : m_vn(NULL)
{

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

    GetLNodeTypeCacheOfStandard();

    GetLNodeTypeCacheOfICD();

    GetDOTypeCacheOfICD();

    GetDOTypeCacheOfStandard();

    GetDATypeCacheOfICD();
    GetDATypeCacheOfStandard();

    GetEnumTypeCacheOfICD();
    GetEnumTypeCacheOfStandard();

    TravellICD(m_vn);

    return true;
}

// 得到标准LNodeType数据
// 加载标准的LNodeTypes(Templates\LNodeTypes（新版396）.xml)
LNodeTypeMap& CCacheMgr::GetLNodeTypeCacheOfStandard()
{
    if (!IsNeededCacheMarkExists("GetLNodeTypeCacheOfStandard"))
    {
        return m_lNodeTypeOfStandard;
    }

    static bool bFinished = false;
    if (bFinished)
    {
        return m_lNodeTypeOfStandard;
    }
    bFinished = true;

    wstring sPath;
    ResolveFilePath(L"Templates\\LNodeTypes（新版396）.xml", sPath);
    LNodeTypeMap& mapLNodeType = m_lNodeTypeOfStandard;
    try
    {
        VTDGen vg;
        if (vg.parseFile(false, WtoA(sPath).c_str()))
        {
            VTDNav *pVn = vg.getNav();
            if (pVn != NULL)
            {
                mapLNodeType.clear();
                if (pVn->toElement(FIRST_CHILD,L"DataTypeTemplates") &&
                    pVn->toElement(FIRST_CHILD,L"LNodeType"))
                {
                    do
                    {
                        wstring sId = VtdGetAttributeValue(pVn, L"id");
                        mapLNodeType.erase(sId);
                        LNodeTypeCache& lNodeType = mapLNodeType[sId];
                        lNodeType.id = sId;
                        lNodeType.lnClass = VtdGetAttributeValue(pVn, L"lnClass");
                        if (pVn->toElement(FIRST_CHILD,L"DO"))
                        {
                            DOMap& mapDo = lNodeType.mapDO;
                            Order& doOrder = lNodeType.doOrder;
                            mapDo.clear();
                            doOrder.clear();
                            do
                            {
                                wstring sDoName = VtdGetAttributeValue(pVn, L"name");
                                doOrder.push_back(sDoName);
                                DOCache& objDo = mapDo[sDoName];
                                objDo.name = sDoName;
                                CStringW stmp = VtdGetAttributeValue(pVn,L"type").c_str();
                                stmp.Trim(L' ');
                                objDo.type = (const wchar_t*)stmp;
                                objDo.eOption = VtdGetAttributeValue(pVn, L"ext:eOption");
                                objDo.offset = pVn->getElementFragment();
                            } while (pVn->toElement(NEXT_SIBLING, L"DO"));
                            pVn->toElement(PARENT);
                        }
                    } while (pVn->toElement(NEXT_SIBLING, L"LNodeType"));
                }
                delete[] pVn->getXML();
                return mapLNodeType;
            }
        }
        else
        {
            delete[] vg.getXML();
        }
    }
    catch (std::exception &e)
    {
        COutputMgr::GetInstance()->Error("CCacheMgr::GetLNodeTypeCacheOfStandard() 异常：%s\r\n", e.what());
    }

    return mapLNodeType;
}

// 得到标准DOType数据
// 加载标准的DOType(Templates\DataTypes（新版396）.xml)
DOTypeMap& CCacheMgr::GetDOTypeCacheOfStandard()
{
    if (!IsNeededCacheMarkExists("GetDOTypeCacheOfStandard"))
    {
        return m_doTypeOfStandard;
    }

    static bool bFinished = false;
    if (bFinished)
    {
        return m_doTypeOfStandard;
    }
    bFinished = true;

    wstring sPath;
    ResolveFilePath(L"Templates\\DataTypes（新版396）.xml", sPath);
    DOTypeMap& mapDOType = m_doTypeOfStandard;
    try
    {
        VTDGen vg;
        if (vg.parseFile(false, WtoA(sPath).c_str()))
        {
            VTDNav *pVtdNav = vg.getNav();
            if (pVtdNav == NULL) 
            {
                return mapDOType;
            }
            mapDOType.clear();
            // 遍历 DOType
            if (pVtdNav->toElement(FIRST_CHILD, L"DataTypeTemplates") &&
                pVtdNav->toElement(FIRST_CHILD, L"DOType"))
            {
                do
                {
                    wstring sId = VtdGetAttributeValue(pVtdNav, L"id");
                    DOTypeCache& doType = mapDOType[sId];
                    doType.id = sId;
                    doType.cdc = VtdGetAttributeValue(pVtdNav, L"cdc");
                    doType.offset = pVtdNav->getElementFragment();
                    DAMap& mapDA = doType.mapDaType;
                    DOMap& mapSDO = doType.mapSDO;
                    Order& sdoOrder = doType.sdoOrder;
                    Order& daOrder = doType.daOrder;
                    // 遍历 DA 
                    if (pVtdNav->toElement(FIRST_CHILD, L"DA"))
                    {
                        do
                        {
                            wstring sDaName = VtdGetAttributeValue(pVtdNav, L"name");
                            daOrder.push_back(sDaName);
                            DACache& objDA = mapDA[sDaName];
                            objDA.name = sDaName;
                            objDA.bType = VtdGetAttributeValue(pVtdNav, L"bType");
                            objDA.type = VtdGetAttributeValue(pVtdNav, L"type");
                            objDA.eOption = VtdGetAttributeValue(pVtdNav, L"ext:eOption");
                            objDA.offset = pVtdNav->getElementFragment();
                        } while (pVtdNav->toElement(NEXT_SIBLING, L"DA"));
                        pVtdNav->toElement(PARENT);
                    }
                    // 遍历 SDO
                    if (pVtdNav->toElement(FIRST_CHILD, L"SDO"))
                    {
                        do
                        {
                            wstring ssdoName = VtdGetAttributeValue(pVtdNav, L"name");
                            wstring ssdoType = VtdGetAttributeValue(pVtdNav, L"type");
                            sdoOrder.push_back(ssdoName);
                            DOCache& objSdo = mapSDO[ssdoName];
                            objSdo.name = ssdoName;
                            objSdo.type = ssdoType;
                            objSdo.offset = pVtdNav->getElementFragment();
                        } while (pVtdNav->toElement(NEXT_SIBLING, L"SDO"));
                        pVtdNav->toElement(PARENT);
                    }
                } while (pVtdNav->toElement(NEXT_SIBLING, L"DOType"));
            }
            delete[] pVtdNav->getXML();

            return mapDOType;
        }
        else
        {
            delete[] vg.getXML();
        }
    }
    catch (std::exception &e)
    {
        COutputMgr::GetInstance()->Error("CCacheMgr::GetLNodeTypeCacheOfStandard() 异常：%s\r\n", e.what());
    }

    return mapDOType;
}

// 得到ICD LNodeType数据
LNodeTypeMap& CCacheMgr::GetLNodeTypeCacheOfICD()
{
    if (!IsNeededCacheMarkExists("GetLNodeTypeCacheOfICD"))
    {
        return m_lNodeTypeOfICD;
    }

    static bool bFinished = false;
    if (bFinished)
    {
        return m_lNodeTypeOfICD;
    }
    bFinished = true;

    // 遍历检测文件中的LNodeType节点
    VTDNav* pVtdNav = m_vn;
    if(pVtdNav==NULL) return m_lNodeTypeOfICD;
    pVtdNav->push();
    pVtdNav->toElement(ROOT);
    LNodeTypeMap& mapLNodeType = m_lNodeTypeOfICD;
    if (pVtdNav->toElement(FIRST_CHILD, L"DataTypeTemplates") &&
        pVtdNav->toElement(FIRST_CHILD, L"LNodeType"))
    {
        do
        {
            wstring sId = VtdGetAttributeValue(pVtdNav, L"id");
            LNodeTypeCache& lNodeType = mapLNodeType[sId];
            lNodeType.id = sId;
            lNodeType.lnClass = VtdGetAttributeValue(pVtdNav, L"lnClass");
            lNodeType.offset = pVtdNav->getElementFragment();
            DOMap& mapDo = lNodeType.mapDO;
            Order& doOrder = lNodeType.doOrder;
            if (pVtdNav->toElement(FIRST_CHILD, L"DO"))
            {
                do
                {
                    wstring sDoName = VtdGetAttributeValue(pVtdNav, L"name");
                    doOrder.push_back(sDoName);
                    DOCache& objDO = mapDo[sDoName];
                    objDO.name = sDoName;
                    objDO.type = VtdGetAttributeValue(pVtdNav, L"type");
                    objDO.offset = pVtdNav->getElementFragment();
                } while (pVtdNav->toElement(NEXT_SIBLING, L"DO"));
                pVtdNav->toElement(PARENT);
            }
        } while (pVtdNav->toElement(NEXT_SIBLING, L"LNodeType"));
        pVtdNav->toElement(ROOT);
    }
    pVtdNav->pop();

    return mapLNodeType;
}

// 得到ICD DOType数据
DOTypeMap& CCacheMgr::GetDOTypeCacheOfICD()
{
    if (!IsNeededCacheMarkExists("GetDOTypeCacheOfICD"))
    {
        return m_doTypeOfICD;
    }

    static bool bFinished = false;
    if (bFinished)
    {
        return m_doTypeOfICD;
    }
    bFinished = true;
    // 遍历检测文件中的LNodeType节点
    VTDNav* pVtdNav = m_vn;
    if(pVtdNav==NULL) return m_doTypeOfICD;
    pVtdNav->push();
    pVtdNav->toElement(ROOT);
    DOTypeMap& mapDOType = m_doTypeOfICD;
    // 遍历 DOType
    if (pVtdNav->toElement(FIRST_CHILD, L"DataTypeTemplates") &&
        pVtdNav->toElement(FIRST_CHILD, L"DOType"))
    {
        do
        {
            wstring sId = VtdGetAttributeValue(pVtdNav, L"id");
            int offset = pVtdNav->getElementFragment();
            DOTypeMap::iterator itDoType = mapDOType.find(sId);
            if (itDoType != mapDOType.end())
            {
                //唯一性检测
                CDotypeCheck::GetInstance()->Report(
                    pChecker->CalcRowNumberFromOffset(offset),
                    pChecker->GetRowContentByOffset(offset),
                    "【DOType:%s】在%d行已存在,不可重复",
                    WtoA(sId).c_str(),
                    pChecker->CalcRowNumberFromOffset(itDoType->second.offset)
                    );
                continue;
            }
            DOTypeCache& doType = mapDOType[sId];
            doType.id = sId;
            doType.cdc = VtdGetAttributeValue(pVtdNav, L"cdc");
            doType.offset = offset;
            DAMap& mapDA = doType.mapDaType;
            DOMap& mapSDO = doType.mapSDO;
            Order& sdoOrder = doType.sdoOrder;
            Order& daOrder = doType.daOrder;
            // 遍历 DA 
            if (pVtdNav->toElement(FIRST_CHILD, L"DA"))
            {
                do 
                {
                    wstring sDaName = VtdGetAttributeValue(pVtdNav, L"name");
                    daOrder.push_back(sDaName);
                    DACache& objDA = mapDA[sDaName];
                    objDA.name = sDaName;
                    objDA.bType = VtdGetAttributeValue(pVtdNav, L"bType");
                    objDA.type = VtdGetAttributeValue(pVtdNav, L"type");
                    objDA.offset = pVtdNav->getElementFragment();
                } while (pVtdNav->toElement(NEXT_SIBLING, L"DA"));
                pVtdNav->toElement(PARENT);
            }
            // 遍历 SDO
            if (pVtdNav->toElement(FIRST_CHILD, L"SDO"))
            {
                do
                {
                    wstring ssdoName = VtdGetAttributeValue(pVtdNav,L"name");
                    wstring ssdoType = VtdGetAttributeValue(pVtdNav,L"type");
                    sdoOrder.push_back(ssdoName);
                    DOCache& objSdo = mapSDO[ssdoName];
                    objSdo.name = ssdoName;
                    objSdo.type = ssdoType;
                    objSdo.offset = pVtdNav->getElementFragment();
                } while (pVtdNav->toElement(NEXT_SIBLING,L"SDO"));
                pVtdNav->toElement(PARENT);
            }
        } while (pVtdNav->toElement(NEXT_SIBLING,L"DOType"));
        pVtdNav->toElement(ROOT);
    }
    pVtdNav->pop();
    return mapDOType;
}

// 得到标准DAType数据
// 加载标准的DAType(Templates\AttributeTypes（新版396）.xml)
DATypeMap& CCacheMgr::GetDATypeCacheOfStandard()
{
    if (!IsNeededCacheMarkExists("GetDATypeCacheOfStandard"))
    {
        return m_daTypeOfStandard;
    }
    static bool bFinished = false;
    if (bFinished)
    {
        return m_daTypeOfStandard;
    }
    bFinished = true;

    wstring sPath;
    ResolveFilePath(L"Templates\\AttributeTypes（新版396）.xml", sPath);

    DATypeMap& mapDAType = m_daTypeOfStandard;
    try
    {
        VTDGen vg;
        if (vg.parseFile(false, WtoA(sPath).c_str()))
        {
            VTDNav *pVtdNav = vg.getNav();
            if (pVtdNav == NULL) 
            {
                return mapDAType;
            }
            mapDAType.clear();
            // 遍历 DOType
            if (pVtdNav->toElement(FIRST_CHILD,L"DataTypeTemplates") &&
                pVtdNav->toElement(FIRST_CHILD,L"DAType"))
            {
                do 
                {
                    wstring sId = VtdGetAttributeValue(pVtdNav,L"id");
                    DATypeCache& daType = mapDAType[sId];
                    daType.id = sId;
                    BDAMap& mapBDA = daType.mapBDA;
                    Order& bdaOrder = daType.bdaOrder;
                    // 遍历 BDA 
                    if (pVtdNav->toElement(FIRST_CHILD,L"BDA"))
                    {
                        do 
                        {
                            wstring sBDAName = VtdGetAttributeValue(pVtdNav,L"name");
                            bdaOrder.push_back(sBDAName);

                            BDACache& objBDA = mapBDA[sBDAName];
                            objBDA.name = sBDAName;
                            objBDA.bType = VtdGetAttributeValue(pVtdNav,L"bType");
                            objBDA.type = VtdGetAttributeValue(pVtdNav,L"type");
                            objBDA.eOption = VtdGetAttributeValue(pVtdNav,L"ext:eOption");
                            objBDA.offset = pVtdNav->getElementFragment();

                        } while (pVtdNav->toElement(NEXT_SIBLING, L"BDA"));

                        pVtdNav->toElement(PARENT);
                    }
                } while (pVtdNav->toElement(NEXT_SIBLING,L"DAType"));
            }
            delete[] pVtdNav->getXML();
            return mapDAType;
        }
        else
        {
            delete[] vg.getXML();
        }
    }
    catch (std::exception &e)
    {
        COutputMgr::GetInstance()->Error("CCacheMgr::GetLNodeTypeCacheOfStandard() 异常：%s\r\n", e.what());
    }
    return mapDAType;
}

// 得到ICD DAType数据
DATypeMap& CCacheMgr::GetDATypeCacheOfICD()
{
    if (!IsNeededCacheMarkExists("GetDATypeCacheOfICD"))
    {
        return m_daTypeOfICD;
    }

    static bool bFinished = false;
    if (bFinished)
    {
        return m_daTypeOfICD;
    }
    bFinished = true;

    // 遍历检测文件中的LNodeType节点
    VTDNav* pVtdNav = m_vn;
    if(pVtdNav==NULL) return m_daTypeOfICD;
    DATypeMap& mapDAType = m_daTypeOfICD;
    // 遍历 DOType
    pVtdNav->push();
    pVtdNav->toElement(ROOT);
    if (pVtdNav->toElement(FIRST_CHILD,L"DataTypeTemplates") &&
        pVtdNav->toElement(FIRST_CHILD,L"DAType"))
    {
        do
        {
            wstring sId = VtdGetAttributeValue(pVtdNav,L"id");
            DATypeCache& daType = mapDAType[sId];
            daType.id = sId;
            daType.offset = pVtdNav->getElementFragment();
            BDAMap& mapBDA = daType.mapBDA;
            Order& bdaOrder = daType.bdaOrder;
            // 遍历 BDA 
            if (pVtdNav->toElement(FIRST_CHILD,L"BDA"))
            {
                do 
                {
                    wstring sBDAName = VtdGetAttributeValue(pVtdNav,L"name");
                    bdaOrder.push_back(sBDAName);
                    BDACache& objBDA = mapBDA[sBDAName];
                    objBDA.name = sBDAName;
                    objBDA.bType = VtdGetAttributeValue(pVtdNav,L"bType");
                    objBDA.type = VtdGetAttributeValue(pVtdNav,L"type");
                    objBDA.eOption = VtdGetAttributeValue(pVtdNav,L"ext:eOption");
                    objBDA.offset = pVtdNav->getElementFragment();
                } while (pVtdNav->toElement(NEXT_SIBLING,L"BDA"));
                pVtdNav->toElement(PARENT);
            }
        } while (pVtdNav->toElement(NEXT_SIBLING,L"DAType"));
        pVtdNav->toElement(ROOT);
    }
    pVtdNav->pop();
    return mapDAType;
}

// 加载标准的EnumType(Templates\EnumTypes（新版396）.xml)
EnumTypeMap& CCacheMgr::GetEnumTypeCacheOfStandard()
{
    if (!IsNeededCacheMarkExists("GetEnumTypeCacheOfStandard"))
    {
        return m_enumTypeOfStandard;
    }

    static bool bFinished = false;
    if (bFinished)
    {
        return m_enumTypeOfStandard;
    }
    bFinished = true;

    wstring sPath;
    ResolveFilePath(L"Templates\\EnumTypes（新版396）.xml", sPath);

    EnumTypeMap& mapEnumType = m_enumTypeOfStandard;
    try
    {
        VTDGen vg;
        if (vg.parseFile(false, WtoA(sPath).c_str()))
        {
            VTDNav *pVtdNav = vg.getNav();
            if (pVtdNav == NULL) 
            {
                return m_enumTypeOfStandard;
            }
            mapEnumType.clear();
            // 遍历 EnumType
            if (pVtdNav->toElement(FIRST_CHILD,L"DataTypeTemplates") &&
                pVtdNav->toElement(FIRST_CHILD,L"EnumType"))
            {
                do 
                {
                    wstring sId = VtdGetAttributeValue(pVtdNav,L"id");

                    EnumType& enumType = mapEnumType[sId];
                    enumType.id = sId;
                    vector<wstring>& vecVal = enumType.vecVal;
                    // 遍历 EnumVal
                    if (pVtdNav->toElement(FIRST_CHILD,L"EnumVal"))
                    {
                        do 
                        {
                            wstring sText = VtdGetTextValue(pVtdNav);
                            vecVal.push_back(sText);
                        } while (pVtdNav->toElement(NEXT_SIBLING,L"EnumVal"));
                        pVtdNav->toElement(PARENT);
                    }
                } while (pVtdNav->toElement(NEXT_SIBLING,L"EnumType"));
            }
            delete[] pVtdNav->getXML();
            return mapEnumType;
        }
        else
        {
            delete[] vg.getXML();
        }
    }
    catch (std::exception &e)
    {
        COutputMgr::GetInstance()->Error("CCacheMgr::GetEnumTypeCacheOfStandard() 异常：%s\r\n", e.what());
    }
    return mapEnumType;
}

// 得到ICD EnumType数据
EnumTypeMap& CCacheMgr::GetEnumTypeCacheOfICD()
{
    if (!IsNeededCacheMarkExists("GetEnumTypeCacheOfICD"))
    {
        return m_enumTypeOfIcd;
    }

    static bool bFinished = false;
    if (bFinished)
    {
        return m_enumTypeOfIcd;
    }
    bFinished = true;

    // 遍历检测文件中的LNodeType节点
    VTDNav* pVtdNav = m_vn;
    if(pVtdNav==NULL)
    {
        return m_enumTypeOfIcd;
    }

    EnumTypeMap& mapEnumType = m_enumTypeOfIcd;
    // 遍历 EnumType
    if (pVtdNav->toElement(FIRST_CHILD,L"DataTypeTemplates") &&
        pVtdNav->toElement(FIRST_CHILD,L"EnumType"))
    {
        do 
        {
            wstring sId = VtdGetAttributeValue(pVtdNav,L"id");
            EnumType& enumType = mapEnumType[sId];
            enumType.id = sId;
            enumType.offset = pVtdNav->getElementFragment();
            vector<wstring>& vecVal = enumType.vecVal;
            // 遍历 EnumVal
            if (pVtdNav->toElement(FIRST_CHILD,L"EnumVal"))
            {
                do
                {
                    wstring sText = VtdGetTextValue(pVtdNav);
                    vecVal.push_back(sText);
                } while (pVtdNav->toElement(NEXT_SIBLING,L"EnumVal"));
                pVtdNav->toElement(PARENT);
            }
        } while (pVtdNav->toElement(NEXT_SIBLING,L"EnumType"));
    }
    pVtdNav->toElement(ROOT);
    return mapEnumType;
}

// 加载标准的DataSet(Templates\DataSet（新版396命名规范）.xml)
DataSetTypeMap& CCacheMgr::GetDataSetTypeOfStandard()
{
    if (!IsNeededCacheMarkExists("GetDataSetTypeOfStandard"))
    {
        return m_datasetTypeOfStandard;
    }

    static bool bFinished = false;
    if (bFinished)
    {
        return m_datasetTypeOfStandard;
    }
    bFinished = true;

    wstring sPath;
    ResolveFilePath(L"Templates\\DataSet（新版396命名规范）.xml", sPath);

    DataSetTypeMap& mapDataSetType = m_datasetTypeOfStandard;
    try
    {
        VTDGen vg;
        if (vg.parseFile(false, WtoA(sPath).c_str()))
        {
            VTDNav *pVtdNav = vg.getNav();
            if (pVtdNav == NULL) 
            {
                return mapDataSetType;
            }
            mapDataSetType.clear();
            // 遍历 EnumType
            if (pVtdNav->toElement(FIRST_CHILD,L"DataSet"))
            {
                do
                {
                    wstring sName = VtdGetAttributeValue(pVtdNav,L"name");
                    DataSetType& dataset = mapDataSetType[sName];
                    dataset.sName = sName;
                    dataset.sDesc = VtdGetAttributeValue(pVtdNav,L"desc");
                } while (pVtdNav->toElement(NEXT_SIBLING,L"DataSet"));
            }
            delete[] pVtdNav->getXML();
            return mapDataSetType;
        }
        else
        {
            delete[] vg.getXML();
        }
    }
    catch (std::exception &e)
    {
        COutputMgr::GetInstance()->Error("CCacheMgr::GetDataSetTypeOfStandard() 异常：%s\r\n", e.what());
    }
    return mapDataSetType;
}

// 加载标准的DataSet(Templates\LDevice（新版396命名规范）.xml)
LDeviceTypeMap& CCacheMgr::GetLDeviceTypeOfStandard()
{
    if (!IsNeededCacheMarkExists("GetLDeviceTypeOfStandard"))
    {
        return m_lDeviceTypeOfStandard;
    }

    static bool bFinished = false;
    if (bFinished)
    {
        return m_lDeviceTypeOfStandard;
    }
    bFinished = true;

    wstring sPath;
    ResolveFilePath(L"Templates\\LDevice（新版396命名规范）.xml", sPath);

    LDeviceTypeMap& mapLDeviceType = m_lDeviceTypeOfStandard;

    try
    {
        VTDGen vg;

        if (vg.parseFile(false, WtoA(sPath).c_str()))
        {
            VTDNav *pVtdNav = vg.getNav();
            if (pVtdNav == NULL) 
            {
                return mapLDeviceType;
            }
            mapLDeviceType.clear();
            // 遍历 EnumType
            if (pVtdNav->toElement(FIRST_CHILD,L"LDevice"))
            {
                do 
                {
                    wstring sName = VtdGetAttributeValue(pVtdNav,L"name");
                    LDeviceType& device = mapLDeviceType[sName];
                    device.sName = sName;
                    device.sDesc = VtdGetAttributeValue(pVtdNav,L"desc");
                } while (pVtdNav->toElement(NEXT_SIBLING,L"LDevice"));
            }
            delete[] pVtdNav->getXML();
            return mapLDeviceType;
        }
        else
        {
            delete[] vg.getXML();
        }
    }
    catch (std::exception &e)
    {
        COutputMgr::GetInstance()->Error("CCacheMgr::GetLDeviceTypeOfStandard() 异常：%s\r\n", e.what());
    }
    return mapLDeviceType;
}

// 加载标准的DataSet(Templates\ReportControl（新版396命名规范）.xml)
ReportControlTypeMap& CCacheMgr::GetReportControlTypeOfStandard()
{
    if (!IsNeededCacheMarkExists("GetReportControlTypeOfStandard"))
    {
        return m_reportControlTypeOfStandard;
    }

    static bool bFinished = false;
    if (bFinished)
    {
        return m_reportControlTypeOfStandard;
    }
    bFinished = true;

    wstring sPath;
    ResolveFilePath(L"Templates\\ReportControl（新版396命名规范）.xml", sPath);

    ReportControlTypeMap& mapReportType = m_reportControlTypeOfStandard;
    try
    {
        VTDGen vg;
        if (vg.parseFile(false, WtoA(sPath).c_str()))
        {
            VTDNav *pVtdNav = vg.getNav();
            if (pVtdNav == NULL) 
            {
                return mapReportType;
            }
            mapReportType.clear();
            // 遍历 EnumType
            if (pVtdNav->toElement(FIRST_CHILD,L"ReportControl"))
            {
                do 
                {
                    wstring sName = VtdGetAttributeValue(pVtdNav,L"name");
                    ReportControlType& device = mapReportType[sName];
                    device.sName = sName;
                    device.sDesc = VtdGetAttributeValue(pVtdNav,L"desc");
                } while (pVtdNav->toElement(NEXT_SIBLING,L"ReportControl"));
            }
            delete[] pVtdNav->getXML();
            return mapReportType;
        }
        else
        {
            delete[] vg.getXML();
        }
    }
    catch (std::exception &e)
    {
        COutputMgr::GetInstance()->Error("CCacheMgr::GetReportControlTypeOfStandard() 异常：%s\r\n", e.what());
    }
    return mapReportType;
}

// LN（新版396定义）
IedTemplateMap& CCacheMgr::GetIedTemplateOfStandard()
{
    if (!IsNeededCacheMarkExists("GetIedTemplateOfStandard"))
    {
        return m_iedTypeOfStandard;
    }

    static bool bFinished = false;
    if (bFinished)
    {
        return m_iedTypeOfStandard;
    }
    bFinished = true;

    wstring sPath;
    ResolveFilePath(L"Templates\\LN（新版396定义）.xml", sPath);

    IedTemplateMap& mapIedTemplate = m_iedTypeOfStandard;
    try
    {
        VTDGen vg;
        VTDNav *pVtdNav = NULL;

        if (!vg.parseFile(false, WtoA(sPath).c_str()) || NULL == (pVtdNav = vg.getNav()))
        {
            delete[] vg.getXML();
            return mapIedTemplate;
        }

        mapIedTemplate.clear();
        // 遍历 IED
        if (pVtdNav->toElement(FIRST_CHILD, L"IED"))
        {
            do
            {
                wstring sName = VtdGetAttributeValue(pVtdNav,L"name");
                wstring sVoltage = VtdGetAttributeValue(pVtdNav,L"voltage");

                IedTemplate& ied = mapIedTemplate[sName+sVoltage];
                ied.sName = sName;
                ied.sType = sVoltage;
                ied.sType = VtdGetAttributeValue(pVtdNav,L"type");
                // 遍历 LDevice
                if (pVtdNav->toElement(FIRST_CHILD,L"LDevice"))
                {
                    LDeviceTemplateMap& mapLDevice = ied.mapLDevice;
                    do
                    {
                        wstring sDesc = VtdGetAttributeValue(pVtdNav,L"desc");
                        wstring sInst = VtdGetAttributeValue(pVtdNav,L"inst");

                        LDeviceTemplate& ld = mapLDevice[sInst];
                        ld.sDesc = sDesc;
                        ld.sInst = sInst;

                        // 遍历 LN
                        if (pVtdNav->toElement(FIRST_CHILD,L"LN"))
                        {
                            LNTemplateMap& mapLN = ld.mapLN;
                            do
                            {
                                wstring sLnClass = VtdGetAttributeValue(pVtdNav,L"lnClass");
                                wstring sLnInst = VtdGetAttributeValue(pVtdNav, L"inst");
                                wstring sDesc = VtdGetAttributeValue(pVtdNav,L"desc");
                                if (sLnClass == L"LLN0")
                                {
                                    sLnInst = L"";
                                }
                                LNTemplate& ln = mapLN[sLnClass+sLnInst];
                                ln.sOption = VtdGetAttributeValue(pVtdNav,L"ext:eOption");
                                ln.sFuntype = VtdGetAttributeValue(pVtdNav,L"funcType");
                                ln.sDesc = sDesc;
                                ln.sLnClass = sLnClass;
                            } while (pVtdNav->toElement(NEXT_SIBLING,L"LN"));
                            pVtdNav->toElement(PARENT);
                        }
                    } while (pVtdNav->toElement(NEXT_SIBLING,L"LDevice"));
                    pVtdNav->toElement(PARENT);
                }
            } while (pVtdNav->toElement(NEXT_SIBLING,L"IED"));
            pVtdNav->toElement(PARENT);
        }
        delete[] pVtdNav->getXML();
        return mapIedTemplate;
    }
    catch (std::exception &e)
    {
        COutputMgr::GetInstance()->Error("CCacheMgr::GetIedTemplateOfStandard() 异常：%s\r\n", e.what());
    }
    return mapIedTemplate;
}


// 遍历整个 ICD xml 文件
void CCacheMgr::TravellICD(VTDNav* pNav)
{
    // 遍历 Communication
    Travell_Communication(pNav);
    // 遍历 IED
    Travell_IED(pNav);
    // 遍历 DataTypeTemplates

}
// 遍历 Communication
void CCacheMgr::Travell_Communication(VTDNav* pNav)
{
    // 哪些检测项启用后需要遍历IED
    if (!IsNeededCacheMarkExists("Travell_Communication"))
    {
        return;
    }
    // End 哪些检测项启用后需要遍历IED
    pNav->push();

    AutoPilot ap;
    ap.selectXPath(L"/SCL/Communication/SubNetwork/ConnectedAP/PhysConn/P");
    ap.bind(pNav);

    while (ap.evalXPath() != -1)
    {
        wstring sType = VtdGetAttributeValue(pNav,L"type");
        wstring sText = VtdGetTextValue(pNav);

        CPhsyconCheck::GetInstance()->CheckPortType_InCacheMgr(sType, sText, pNav->getElementFragment());
    }

    pNav->pop();
}
// 遍历IED
void CCacheMgr::Travell_IED(VTDNav* pNav)
{
    // 哪些检测项启用后需要遍历IED
    if (!IsNeededCacheMarkExists("Travell_IED"))
    {
        return;
    }
    // End 哪些检测项启用后需要遍历IED

    if (!pNav->toElement(FIRST_CHILD,L"IED"))
    {
        return;
    }

    wstring sIEDName = VtdGetAttributeValue(pNav,L"name");
    CIedCheck::GetInstance()->CheckIedName_InCacheMgr(sIEDName, pNav->getElementFragment());

    ICDCache icd;
    icd.offset = pNav->getElementFragment();
    // 遍历 Services
    Travell_Services(pNav,icd.services);
    // 遍历 AccessPoint
    Travell_AccessPoint(pNav,icd);

    CGsSmvCheck::GetInstance()->CheckGseSmv_InCacheMgr(icd);

    CConfDataSetCheck::GetInstance()->CheckDataset_InCacheMgr(icd);

    CConfReportCheck::GetInstance()->CheckReportCB_InCacheMgr(icd);

    CConfLogCheck::GetInstance()->CheckLogCB_InCacheMgr(icd);

    CLnValidCheck::GetInstance()->CheckValid_InCacheMgr(icd);

    pNav->toElement(PARENT);
}

void CCacheMgr::Travell_AccessPoint(VTDNav* pNav,ICDCache& icd)
{
    // 哪些检测项启用后需要遍历 AccessPoint
    if (!IsNeededCacheMarkExists("Travell_AccessPoint"))
    {
        return;
    }
    // End 哪些检测项启用后需要遍历 AccessPoint
    pNav->push();
    // 遍历 Server/LDevice    
    if(!pNav->toElement(FIRST_CHILD,L"AccessPoint"))
    {
        pNav->pop();
        return;
    }
    do
    {
        wstring sAccessPoint = VtdGetAttributeValue(pNav, L"name");

        if(!pNav->toElement(FIRST_CHILD,L"Server"))
        {
            continue;
        }
        do 
        {
            bool bHasDevice = false;
            bool bLDevice = pNav->toElement(FIRST_CHILD,L"LDevice");
            while (bLDevice)
            {
                if (!bHasDevice)
                {
                    bHasDevice = true;
                }
                LDeviceCache ldevice;
                ldevice.sAccessPoint = sAccessPoint;
                ldevice.sLdInst = VtdGetAttributeValue(pNav, L"inst");
                ldevice.sDesc = VtdGetAttributeValue(pNav, L"desc");
                ldevice.offset = pNav->getElementFragment();

                CLdDescCheck::GetInstance()->CheckLDeviceDesc_InCacheMgr(ldevice, pNav->getElementFragment());

                CLdValidCheck::GetInstance()->CheckName_InCacheMgr(ldevice.sLdInst,pNav->getElementFragment());

                // 遍历 LN
                Travell_LN(pNav, ldevice);
                // 遍历 LN0
                Travell_LN0(pNav, ldevice, icd.services);

                CServerCheck::GetInstance()->CheckLLN0LPHD_InCacheMgr(ldevice);

                icd.mapLDevice.insert(make_pair(ldevice.sLdInst,ldevice));

                bLDevice = pNav->toElement(NEXT_SIBLING,L"LDevice");
            }
            if (!bHasDevice)
            {
                // Server对象中至少应包含一个 LDevice 对象
                CServerCheck::GetInstance()->CheckhasLDevice_InCacheMgr(bHasDevice,pNav->getElementFragment());
            }
            else
            {
                pNav->toElement(PARENT);
            }
        } while (pNav->toElement(NEXT_SIBLING,L"Server"));
        pNav->toElement(PARENT);
    } while (pNav->toElement(NEXT_SIBLING,L"AccessPoint"));
    pNav->pop();
}

// 遍历 Services
void CCacheMgr::Travell_Services(VTDNav* pNav, ServicesCache& services)
{
    // 哪些检测项启用后需要遍历 Services
    if (!IsNeededCacheMarkExists("Travell_Services"))
    {
        return;
    }
    // End 哪些检测项启用后需要遍历 Services

    if (!pNav->toElement(FIRST_CHILD,L"Services"))
    {
        return;
    }
    services.offset = pNav->getElementFragment();
    // ConfDataSet
    if (pNav->toElement(FIRST_CHILD, L"ConfDataSet"))
    {
        wstring sMax = VtdGetAttributeValue(pNav, L"max");
        wstring sMaxAttr = VtdGetAttributeValue(pNav, L"maxAttributes");
        services.dataset_offset = pNav->getElementFragment();
        if (sMax.empty() == false)
        {
            services.dataset_max = _wtoi(sMax.c_str());
        }
        if (sMaxAttr.empty() == false)
        {
            services.dataset_maxAttr = _wtoi(sMaxAttr.c_str());
            if (services.dataset_maxAttr > 256)
            {
                CConfFcdaCheck::GetInstance()->Report(
                    pChecker->CalcRowNumberFromOffset(services.dataset_offset),
                    pChecker->GetRowContentByOffset(services.dataset_offset),
                    "maxAttribue的值不能大于256 maxAttribue=%d ",
                    services.dataset_maxAttr
                    );
            }
        }

        pNav->toElement(PARENT);
    }
    // ConfReportControl
    if (pNav->toElement(FIRST_CHILD, L"ConfReportControl"))
    {
        wstring sMax = VtdGetAttributeValue(pNav, L"max");
        services.report_offset = pNav->getElementFragment();
        if (sMax.empty() == false)
        {
            services.report_max = _wtoi(sMax.c_str());
        }
        pNav->toElement(PARENT);
    }
    // ConfLogControl
    if (pNav->toElement(FIRST_CHILD, L"ConfLogControl"))
    {
        wstring sMax = VtdGetAttributeValue(pNav, L"max");
        services.log_offset = pNav->getElementFragment();
        if (sMax.empty() == false)
        {
            services.log_max = _wtoi(sMax.c_str());
        }
        pNav->toElement(PARENT);
    }
    pNav->toElement(PARENT);
}

void CCacheMgr::Travell_LN0(VTDNav* pNav, LDeviceCache& ldevice, ServicesCache& services)
{
    if (!IsNeededCacheMarkExists("Travell_LN0"))
    {
        return;
    }

    if (pNav->toElement(FIRST_CHILD, L"LN0"))
    {
        wstring sLnClass = VtdGetAttributeValue(pNav, L"lnClass");
        wstring sLnInst = VtdGetAttributeValue(pNav, L"inst");
        wstring sLnType = VtdGetAttributeValue(pNav, L"lnType");
        wstring sDesc = VtdGetAttributeValue(pNav, L"desc");
        wstring sPrefix = VtdGetAttributeValue(pNav, L"prefix");

        LNCache& ln = ldevice.mapLN[sLnClass];
        ln.offset = pNav->getElementFragment();
        ln.sDesc = sDesc;
        ln.sInst = sLnInst;
        ln.sLnClass = sLnClass;
        ln.sLnType = sLnType;
        ln.sPrefix = sPrefix;

        if (sLnClass == L"LLN0")
        {
            ldevice.bLLN0 = true;
        }

        DOMap mapDo;
        bool bPass = CLDdoiCheck::GetInstance()->CheckLnTypeValid_InCacheMgr(ln, mapDo);
        // 遍历 DOI
        if (bPass)
        {
            Travell_DOI(pNav, mapDo);
        }
        // 遍历 ReportControl
        if (pNav->toElement(FIRST_CHILD, L"ReportControl"))
        {
            do
            {
                wstring sName = VtdGetAttributeValue(pNav, L"name");
                wstring sDatasetName = VtdGetAttributeValue(pNav, L"datSet");
                ldevice.mapDsToCB[sDatasetName] = L"ReportControl";
                ldevice.nReportCB++;

                CReportCtrlCheck::GetInstance()->CheckName_InCacheMgr(sName, pNav->getElementFragment());

            } while (pNav->toElement(NEXT_SIBLING, L"ReportControl"));

            pNav->toElement(PARENT);
        }
        // 遍历 LogControl
        if (pNav->toElement(FIRST_CHILD, L"LogControl"))
        {
            do
            {
                wstring sDatasetName = VtdGetAttributeValue(pNav, L"datSet");
                ldevice.mapDsToCB[sDatasetName] = L"LogControl";
                ldevice.nLogCB++;
            } while (pNav->toElement(NEXT_SIBLING, L"LogControl"));
            pNav->toElement(PARENT);
        }
        // 遍历 SettingControl
        if (pNav->toElement(FIRST_CHILD, L"SettingControl"))
        {
            do
            {
                wstring sDatasetName = VtdGetAttributeValue(pNav, L"datSet");
                ldevice.mapDsToCB[sDatasetName] = L"SettingControl";
            } while (pNav->toElement(NEXT_SIBLING, L"SettingControl"));
            pNav->toElement(PARENT);
        }
        // 遍历 GSEControl
        if (pNav->toElement(FIRST_CHILD, L"GSEControl"))
        {
            do
            {
                wstring sDatasetName = VtdGetAttributeValue(pNav, L"datSet");
                ldevice.mapDsToCB[sDatasetName] = L"GSEControl";
                ldevice.bHasGoose = true;
            } while (pNav->toElement(NEXT_SIBLING, L"GSEControl"));
            pNav->toElement(PARENT);
        }
        // 遍历 SampledValueControl
        if (pNav->toElement(FIRST_CHILD, L"SampledValueControl"))
        {
            do
            {
                wstring sDatasetName = VtdGetAttributeValue(pNav, L"datSet");
                ldevice.mapDsToCB[sDatasetName] = L"SampledValueControl";
                ldevice.bHasSmv = true;
            } while (pNav->toElement(NEXT_SIBLING, L"SampledValueControl"));
            pNav->toElement(PARENT);
        }
        // 遍历 DataSet
        Travell_DataSet(pNav, ldevice, services.dataset_maxAttr);
        // 遍历 Inputs
        if (pNav->toElement(FIRST_CHILD, L"Inputs"))
        {
            // IED中不应包含Inputs这类代表GOOSE/SV连线的信息
            CRedInputsCheck::GetInstance()->CheckInputs_InCacheMgr(pNav->getElementFragment());
            pNav->toElement(PARENT);
        }
        pNav->toElement(PARENT);
    }
}

void CCacheMgr::Travell_DataSet(VTDNav* pNav, LDeviceCache& ldevice, int maxFcda)
{
    if (!IsNeededCacheMarkExists("Travell_DataSet"))
    {
        return;
    }

    if (!pNav->toElement(FIRST_CHILD, L"DataSet"))
    {
        return;
    }
    DSTOCB& ds2cb = ldevice.mapDsToCB;
    DATASETMAP& mapDataset = ldevice.mapDataset;

    do
    {
        wstring sDatasetName = VtdGetAttributeValue(pNav, L"name");
        wstring sCBType = ds2cb[sDatasetName];

        DatasetCache& dateset = mapDataset[sDatasetName];
        dateset.sName = sDatasetName;
        dateset.sAccessPoint = ldevice.sAccessPoint;
        dateset.sLDevice = ldevice.sLdInst;
        dateset.offset = pNav->getElementFragment();

        CDataSetCheck::GetInstance()->CheckName_InCacheMgr(dateset, pNav->getElementFragment());
        // 遍历 FCDA
        if (pNav->toElement(FIRST_CHILD, L"FCDA"))
        {
            map<wstring,wstring> mapFcdaKey;
            do
            {
                FcdaCache fcda;
                fcda.sDataset = sDatasetName;
                fcda.offset = pNav->getElementFragment();
                fcda.sLdInst = VtdGetAttributeValue(pNav, L"ldInst");
                fcda.sLnClass = VtdGetAttributeValue(pNav, L"lnClass");
                fcda.sLnInst = VtdGetAttributeValue(pNav, L"lnInst");
                // Goose 配置到 da,其它配置到 do
                int iDoToken = pNav->getAttrVal(L"doName");
                int iDaToken = pNav->getAttrVal(L"daName");
                fcda.bHasDoName =  iDoToken != -1;
                fcda.bHasDaName = iDaToken != -1;
                fcda.sDoName = VtdGetString(pNav, iDoToken);
                fcda.sDaName = VtdGetString(pNav, iDaToken);
                fcda.sFc = VtdGetAttributeValue(pNav, L"fc");
                fcda.sPrefix = VtdGetAttributeValue(pNav, L"prefix");
                dateset.vecFcda.push_back(fcda);

                wstring sKey = fcda.sLdInst + L"/" + fcda.sPrefix + fcda.sLnClass + fcda.sLnInst + L"." + fcda.sDoName;
                if (!fcda.sDaName.empty())
                {
                    sKey += L".";
                    sKey += fcda.sDaName;
                }
                if (mapFcdaKey.find(sKey) != mapFcdaKey.end())
                {
                    //唯一性检测
                    CFcdaValidCheck::GetInstance()->Report(
                        pChecker->CalcRowNumberFromOffset(fcda.offset),
                        pChecker->GetRowContentByOffset(fcda.offset),
                        "【FCDA:%s】已存在",
                        WtoA(sKey).c_str()
                        );
                    continue;
                }
                mapFcdaKey.insert(make_pair(sKey, sKey));
                // 不应跨 ld
                CFcdaLdValidCheck::GetInstance()->CheckCrossLDevice(fcda, ldevice.sLdInst);
                // fcda 引用有效性检查
                bool bPass = CFcdaValidCheck::GetInstance()->CheckValid_InCacheMgr(fcda, ldevice);
                if(!bPass)
                {
                   CFcdaValidCheck::GetInstance()->Report(
                        pChecker->CalcRowNumberFromOffset(fcda.offset),
                        pChecker->GetRowContentByOffset(fcda.offset),
                        "数据集配置的FCDA所指向的模型中不存在对应的数据对象和数据属性:\
                        AccessPoint name=%s LDevice inst=%s, DataSet name=%s, FCDA=%s",
                        WtoA(ldevice.sAccessPoint).c_str(),
                        WtoA(ldevice.sLdInst).c_str(),
                        WtoA(dateset.sName).c_str(),
                        WtoA(sKey).c_str()
                        );
                }
                // 数据集有效性检查
                CDataSetTypeValidCheck::GetInstance()->CheckValid_InCacheMgr(sCBType, fcda);

            } while (pNav->toElement(NEXT_SIBLING, L"FCDA"));
            pNav->toElement(PARENT);
        }
        CConfFcdaCheck::GetInstance()->CheckFcdaCount_InCacheMgr(dateset, maxFcda);
    } while (pNav->toElement(NEXT_SIBLING, L"DataSet"));
    pNav->toElement(PARENT);
}

void CCacheMgr::Travell_LN(VTDNav* pNav, LDeviceCache& ldevice)
{
    if (!IsNeededCacheMarkExists("Travell_LN"))
    {
        return;
    }

    if (!pNav->toElement(FIRST_CHILD,L"LN"))
    {
        return;
    }
    do
    {
        wstring sLnType = VtdGetAttributeValue(pNav, L"lnType");
        wstring sLnClass = VtdGetAttributeValue(pNav, L"lnClass");
        wstring sLnInst = VtdGetAttributeValue(pNav, L"inst");
        wstring sPrefix = VtdGetAttributeValue(pNav, L"prefix");
        wstring sDesc = VtdGetAttributeValue(pNav, L"desc");

        LNCache& ln = ldevice.mapLN[sPrefix + sLnClass + sLnInst];
        ln.offset = pNav->getElementFragment();
        ln.sLdInst = ldevice.sLdInst;
        ln.sDesc = sDesc;
        ln.sInst = sLnInst;
        ln.sLnClass = sLnClass;
        ln.sLnType = sLnType;
        ln.sPrefix = sPrefix;

        m_curLN = ln;

        DOMap mapDo;
        CLDdoiCheck::GetInstance()->CheckLnTypeValid_InCacheMgr(ln, mapDo);

        CLdDescCheck::GetInstance()->CheckLNDesc_InCacheMgr(sDesc, pNav->getElementFragment());
        // 拼接可能的信号路径 
        // ldInst/prefix+lnclass+lnInst.doname.sdiname.sdaname
        wstring sKey = ldevice.sLdInst + L"/" + sPrefix + sLnClass + sLnInst;

        Travell_DOI(pNav, mapDo);

        if(!ldevice.bLPHD && sLnClass == L"LPHD")
        {
            ldevice.bLPHD = true;
        }
    } while (pNav->toElement(NEXT_SIBLING,L"LN"));

    pNav->toElement(PARENT);
}

void CCacheMgr::Travell_DOI(VTDNav* pNav, DOMap& mapDo)
{
    if (!IsNeededCacheMarkExists("Travell_DOI"))
    {
        return;
    }
    if (!pNav->toElement(FIRST_CHILD, L"DOI"))
    {
        return;
    }
    do
    {
        DOICache& doi = m_curDoi;
        doi.sname = VtdGetAttributeValue(pNav, L"name");
        doi.sdesc = VtdGetAttributeValue(pNav, L"desc");
        doi.offset = pNav->getElementFragment();

        DOTypeCache doType;
        bool bPass = CLDdoiCheck::GetInstance()->CheckDOTypeValid_InCacheMgr(doi, mapDo, doType);
        if (!bPass)
        {
            continue;
        }

        SdiCache sdi;
        sdi.pos = SdiCache::IN_DO;
        sdi.mapDaType = doType.mapDaType;
        sdi.mapSDO = doType.mapSDO;
        // 遍历 DAI
        Travell_DAI(pNav, doi, sdi);
        // 遍历 SDI
        Travell_SDI(pNav, doi, sdi);

    } while (pNav->toElement(NEXT_SIBLING, L"DOI"));
    pNav->toElement(PARENT);
}

void CCacheMgr::Travell_DAI(VTDNav* pNav, DOICache& doi, SdiCache& sdi)
{
    if (!IsNeededCacheMarkExists("Travell_DAI"))
    {
        return;
    }

    if (!pNav->toElement(FIRST_CHILD, L"DAI"))
    {
        return;
    }
    do 
    {
        wstring sName  = VtdGetAttributeValue(pNav, L"name");

        CLDdoiCheck::GetInstance()->CheckDATypeValid_InCacheMgr(pNav->getElementFragment(), sName, sdi);

        // 遍历 Val
        if (sName == L"dU")
        {
            if (pNav->toElement(FIRST_CHILD, L"Val"))
            {
                wstring sVal = VtdGetTextValue(pNav);

                CDoiValidCheck::GetInstance()->CheckDescAnddU_InCacheMgr(m_curLN, doi, sVal, pNav->getElementFragment());

                pNav->toElement(PARENT);
            }
        }
    } while (pNav->toElement(NEXT_SIBLING, L"DAI"));

    pNav->toElement(PARENT);
}

void CCacheMgr::Travell_SDI(VTDNav* pNav, DOICache& doi, SdiCache& sdi)
{
    if (!IsNeededCacheMarkExists("Travell_SDI"))
    {
        return;
    }

    if (!pNav->toElement(FIRST_CHILD, L"SDI"))
    {
        return;
    }
    do
    {
        wstring sName  = VtdGetAttributeValue(pNav, L"name");
        wstring sDesc  = VtdGetAttributeValue(pNav, L"desc");

        SdiCache sdiSub = sdi;
        bool bPass = CLDdoiCheck::GetInstance()->CheckSDITypeValid_InCacheMgr(pNav->getElementFragment(), sName, sdiSub);
        if (!bPass)
        {
            continue;
        }
        doi.sdesc = sDesc;
        // 遍历 DAI
        Travell_DAI(pNav, doi, sdiSub);
        // 遍历 SDI
        Travell_SDI(pNav, doi, sdiSub);

    } while (pNav->toElement(NEXT_SIBLING, L"SDI"));

    pNav->toElement(PARENT);
}