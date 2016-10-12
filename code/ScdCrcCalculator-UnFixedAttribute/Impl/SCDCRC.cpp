
#include "SCDCRC.h"
#include <atlstr.h>
#include <algorithm>
#include <ShlObj.h>
#include <atltime.h>

#include "..\..\ScdCheckCommon\charconv.h"

vector<std::wstring> split_string(const std::wstring& sSource, const std::wstring& sDelim, bool ignore_blank)
{    
    vector<wstring> vecDest;

    wstring::size_type pos = sSource.find(sDelim);
    wstring::size_type begin = 0;
    bool first = true;

    while (true)
    {
        if (pos == sSource.npos)
        {
            if (begin < sSource.size())
            {
                vecDest.push_back(sSource.substr(begin));
            }

            return vecDest;
        }
        else
        {
            if (pos > begin || (!first && !ignore_blank))
            {
                vecDest.push_back(sSource.substr(begin, pos - begin));
            }

            begin = pos + sDelim.size();
            pos = sSource.find(sDelim, begin);

            first = false;
        }
    }

    return vecDest;
}

CString SortAttrib(CMarkup& xml, bool bCRC)
{
    vector<wstring> vecKey;
    map<wstring, wstring> mapKV;
    int iAttrib = 0;
    while (TRUE)
    {
        wstring sName = xml.GetAttribName(iAttrib++);
        CString sValue = xml.GetAttrib(sName).c_str();
        // sValue.Trim();
        sValue = xml.EscapeText((LPCTSTR)sValue).c_str();

        if (sName.empty())
        {
            break;
        }
        // 去掉描述
        if (bCRC)
        {
            if (sName == L"desc")
            {
                continue;
            }
        }

        vecKey.push_back(sName);
        mapKV[sName] = sValue;
    }
    if (vecKey.size() == 0)
    {
        return _T("");
    }
    // 将属性排序后写入
    sort(vecKey.begin(), vecKey.end());
    wstring sAttribList;
    for (unsigned int iKey = 0; iKey < vecKey.size(); iKey++)
    {
        wstring sKey = vecKey[iKey];
        CString sValue = mapKV[sKey].c_str();
        sAttribList.append(sKey);
        sAttribList.append(_T("=\""));
        sAttribList.append((LPCTSTR)sValue);
        if (bCRC)
        {
            sAttribList.append(_T("\""));
        }
        else
        {
            sAttribList.append(_T("\" "));
        }
    }
    return sAttribList.c_str();
}

bool TravellXml(CMarkup& xml, CString& sXml, bool bCRC)
{
    bool bAppend = false;
    while(xml.FindElem())
    {
        xml.SavePos(L"Pos1");
        bool bHasChild = xml.FindChildElem();
        xml.RestorePos(L"Pos1");

        CString csElemData = xml.GetData().c_str();
        csElemData.Trim();

        wstring sElemTag = xml.GetTagName();
        wstring sElemData = (LPCTSTR)csElemData;
        wstring sdU = xml.GetAttrib(L"name");
        if(sdU == _T("dU"))
        {
            continue;
        }
        bAppend = true;
        if (sElemTag == L"DOI" || sElemTag == L"DAI" || sElemTag == L"SDI")
        {
            bool bremove = xml.RemoveAttrib(L"desc");
        }

        wstring sAttrList = SortAttrib(xml,bCRC);
        
        CString sFormat;
        if (bHasChild)
        {
            if (bCRC)
            {
                sFormat.Format(_T("<%s%s>"), sElemTag.c_str(), sAttrList.c_str());
            }
            else
            {
                sFormat.Format(_T("<%s %s>\n"), sElemTag.c_str(), sAttrList.c_str());
            }
            sXml += sFormat;

            xml.IntoElem();
            bool bAdd = TravellXml(xml, sXml, bCRC);
            xml.OutOfElem();

            if (bAdd)
            {
                if (bCRC)
                {
                    sFormat.Format(_T("</%s>"), sElemTag.c_str());
                }
                else
                {
                    sFormat.Format(_T("</%s>\n"), sElemTag.c_str());
                }
                sXml += sFormat;
            }
            else
            {
                if (bCRC)
                {
                    sXml.Insert(sXml.GetLength()-1, _T("/"));
                }
                else
                {
                    sXml.Insert(sXml.GetLength()-2, _T("/"));
                }
            }
        }
        else if (!sElemData.empty())
        {
            if (bCRC)
            {
                sFormat.Format(_T("<%s%s>"), sElemTag.c_str(), sAttrList.c_str());
            }
            else
            {
                sFormat.Format(_T("<%s %s>"), sElemTag.c_str(), sAttrList.c_str());
            }

            sXml += sFormat;
            sElemData = xml.EscapeText(sElemData);
            sXml += sElemData.c_str();
            if (bCRC)
            {
                sFormat.Format(_T("</%s>"), sElemTag.c_str());
            }
            else
            {
                sFormat.Format(_T("</%s>\n"), sElemTag.c_str());
            }
            sXml += sFormat;
        }
        else
        {
            if (bCRC)
            {
                sFormat.Format(_T("<%s%s/>"), sElemTag.c_str(), sAttrList.c_str());
            }
            else
            {
                sFormat.Format(_T("<%s %s/>\n"), sElemTag.c_str(), sAttrList.c_str());
            }
            sXml += sFormat;
        }
    }
    return bAppend;
}

CSCDCRC::CSCDCRC(void):m_IEDMap(m_iedmap)
{

}


CSCDCRC::~CSCDCRC(void)
{

}
/**
* @brief    Load简易说明
* 
* 加载SCD
*
* @return   bool    布尔值
* @author   修黎明 
* @date     2013/09/01
*/
bool CSCDCRC::Load(const wchar_t* pwPath)
{
    m_sScd = PathFindFileName(pwPath);

    CString sDoc = pwPath;

    if (!m_xmlMarkUp.SetDoc(pwPath) && !m_xmlMarkUp.Load(pwPath))
    {
        throw std::runtime_error("xml文档加载失败");
    }

    if (DataTypeTemplateImport() 
        && CommImport() 
        && IedImport() 
        && WriteXml()
        && WriteCCD())
    {
        return true;
    }
    else
    {
        throw std::runtime_error("xml文档解析失败");
        return false;
    }
}

/**
* @brief    CommImport简易说明
* 
* 解析通信部分
*
* @return   bool    布尔值
* @author   修黎明 
* @date     2013/08/31
*/
bool CSCDCRC::CommImport()
{
    log_info(_T("执行 CommImport"));
    m_xmlMarkUp.ResetMainPos();
    while (m_xmlMarkUp.FindChildElem(_T("Communication")))
    {
        m_xmlMarkUp.IntoElem();
        while (m_xmlMarkUp.FindChildElem(_T("SubNetwork")))
        {
            m_xmlMarkUp.IntoElem();
            wstring sSubNetName = m_xmlMarkUp.GetAttrib(L"name");
            wstring sSubNetType = m_xmlMarkUp.GetAttrib(L"type");

            while (m_xmlMarkUp.FindChildElem(_T("ConnectedAP")))
            {
                m_xmlMarkUp.IntoElem();

                // ConnectedAP 属性
                AttributesMap mapConnectedAPAttributes;
                wstring attributeName, attributeValue;
                for (int index = 0; ; index++)
                {
                    wstring attributeName = m_xmlMarkUp.GetAttribName(index);
                    if (attributeName.size() != 0)
                    {
                        attributeValue = m_xmlMarkUp.GetAttrib(attributeName);
                        mapConnectedAPAttributes[attributeName] = attributeValue;
                    }
                    else
                        break;
                }

                // connectedAp下的私有元素
                wstring sPrivateString;
                while(m_xmlMarkUp.FindChildElem(_T("Private")))
                {
                    m_xmlMarkUp.IntoElem();
                    sPrivateString += m_xmlMarkUp.GetSubDoc();
                    m_xmlMarkUp.OutOfElem();
                }
                // connectedAp下的PhysConn元素
                wstring sPhysConnString;
                m_xmlMarkUp.ResetChildPos();
                while(m_xmlMarkUp.FindChildElem(_T("PhysConn")))
                {
                    m_xmlMarkUp.IntoElem();
                    sPhysConnString += m_xmlMarkUp.GetSubDoc();
                    m_xmlMarkUp.OutOfElem();
                }
                m_xmlMarkUp.ResetChildPos();
                while (m_xmlMarkUp.FindChildElem(_T("GSE")) || m_xmlMarkUp.FindChildElem(_T("SMV")))
                {
                    m_xmlMarkUp.IntoElem();

                    // 取得GSE或SMV属性
                    AttributesMap mapGSEOrSMVAttributes;
                    for (int index = 0; ; index++)
                    {
                        wstring attributeName = m_xmlMarkUp.GetAttribName(index);
                        if (attributeName.size() != 0)
                        {
                            attributeValue = m_xmlMarkUp.GetAttrib(attributeName);
                            mapGSEOrSMVAttributes[attributeName] = attributeValue;
                        }
                        else
                            break;
                    }
                    wstring sApConnectedAPString = _T("<ConnectedAP ");
                    AttributesMap::iterator itCB = mapConnectedAPAttributes.begin();
                    for (; itCB != mapConnectedAPAttributes.end(); itCB++)
                    {
                        if (itCB->first == L"desc")
                        {
                            continue;
                        }
                        sApConnectedAPString += _T(" ") + CString(itCB->first.c_str()) + _T("=\"") + CString(itCB->second.c_str()) + _T("\"");
                    }
                    sApConnectedAPString += _T("/>");


                    OutCbRef cbref;
                    cbref.sApConnectedAPString = sApConnectedAPString;
                    cbref.bConnectedAp = true;

                    if (m_xmlMarkUp.GetTagName() == _T("GSE"))
                    {
                        cbref.sCbType = _T("GSE");
                        cbref.sPath = mapConnectedAPAttributes[L"iedName"] + mapGSEOrSMVAttributes[L"ldInst"] + _T("/LLN0$GO$") + mapGSEOrSMVAttributes[L"cbName"]; 
                    }
                    else if (m_xmlMarkUp.GetTagName() == _T("SMV"))
                    {
                        cbref.sCbType = _T("SMV");
                        cbref.sPath = mapConnectedAPAttributes[L"iedName"] + mapGSEOrSMVAttributes[L"ldInst"] + _T("/LLN0$SV$") + mapGSEOrSMVAttributes[L"cbName"]; 
                    }
                    else
                    {
                        cbref.bConnectedAp = false;
                    }
                    cbref.sAp = mapConnectedAPAttributes[L"apName"];
                    cbref.sApPrivateString = sPrivateString;
                    cbref.sPhysConnString = sPhysConnString;
                    cbref.sCbString = m_xmlMarkUp.GetSubDoc();

                    IEDMap::iterator it = m_iedmap.find(mapConnectedAPAttributes[L"iedName"]);
                    if (it == m_iedmap.end())
                    {
                        IED ied;
                        ied.mapOutcb.insert(std::make_pair(cbref.sPath, cbref));
                        m_iedmap.insert(std::make_pair(mapConnectedAPAttributes[L"iedName"], ied));
                    }
                    else
                    {
                        it->second.mapOutcb.insert(std::make_pair(cbref.sPath, cbref));
                    }
                    m_xmlMarkUp.OutOfElem();
                }
                m_xmlMarkUp.OutOfElem();
            }
            m_xmlMarkUp.OutOfElem();
        }
        m_xmlMarkUp.OutOfElem();
    }
    return true;
}

BOOL CSCDCRC::hasSignal(IED& ied,wstring sKey)
{
    vector<wstring>& vecInSignal = ied.vecInsignal;

    if(find(vecInSignal.begin(),vecInSignal.end(),sKey) != vecInSignal.end())
    {
        return TRUE;
    }

    OutCBMap::iterator itCB = ied.mapOutcb.begin();
    while(itCB!=ied.mapOutcb.end())
    {
        vector<wstring>& vecOutSignal = itCB->second.vecFcdaKey;
        if(find(vecOutSignal.begin(),vecOutSignal.end(),sKey) != vecOutSignal.end())
        {
            return TRUE;
        }

        itCB++;
    }

    return FALSE;
}
/**
* @brief    IedImport简易说明
* 
* 解析IED部分
*
* @return   bool    布尔值
* @author   修黎明 
* @date     2013/09/1
*/
bool CSCDCRC::TravellDAI(IED& ied, wstring sPath, wstring &sDoDesc)
{
    while(m_xmlMarkUp.FindChildElem(_T("DAI")))
    {
        m_xmlMarkUp.IntoElem();
        wstring sDaiName = m_xmlMarkUp.GetAttrib(_T("name"));
        wstring sKey = sPath+_T(".") + sDaiName;
        if (hasSignal(ied,sKey))
        {
            ied.mapSignalDai[sKey] = m_xmlMarkUp.GetSubDoc();
        }
        ied.mapSignalDaiDesc[sKey] = sDoDesc;
        m_xmlMarkUp.OutOfElem();
    }
    return true;
}

bool CSCDCRC::TravellSDI(IED& ied, wstring sPath, wstring &sDoDesc)
{
    while(m_xmlMarkUp.FindChildElem(_T("SDI")))
    {
        m_xmlMarkUp.IntoElem();
        wstring sSdiName = m_xmlMarkUp.GetAttrib(_T("name"));
        wstring sSdiDesc = m_xmlMarkUp.GetAttrib(_T("desc"));
        wstring sKey = sPath+_T(".") + sSdiName;
        if (hasSignal(ied,sKey))
        {
            ied.mapSignalDai[sKey] = m_xmlMarkUp.GetSubDoc();
        }
        ied.mapSignalDaiDesc[sKey] = sSdiDesc;
        // DOI 下遍历 DAI SDI
        TravellDAI(ied, sKey, sSdiDesc);
        m_xmlMarkUp.ResetChildPos();
        TravellSDI(ied, sKey, sSdiDesc);

        m_xmlMarkUp.OutOfElem();
    }
    return true;
}

bool CSCDCRC::TravellDOI(IED& ied, wstring sPath)
{
    while(m_xmlMarkUp.FindChildElem(_T("DOI")))
    {
        m_xmlMarkUp.IntoElem();

        wstring sDoName = m_xmlMarkUp.GetAttrib(_T("name"));
        wstring sDoDesc = m_xmlMarkUp.GetAttrib(_T("desc"));
        // 建立 ldInst/prefix+lnclass+lnInst.doname
        wstring sKey = sPath+_T(".")+sDoName;
        if (hasSignal(ied,sKey))
        {
            ied.mapSignalDai[sKey] = m_xmlMarkUp.GetSubDoc();
        }
        ied.mapSignalDaiDesc[sKey] = sDoDesc;
        // DOI 下遍历 DAI SDI
        TravellDAI(ied, sKey, sDoDesc);
        m_xmlMarkUp.ResetChildPos();
        TravellSDI(ied, sKey, sDoDesc);

        m_xmlMarkUp.OutOfElem();
    }
    return true;
}

bool CSCDCRC::IedImport()
{
    log_info(_T("执行 IedImport"));
    m_xmlMarkUp.ResetMainPos();
    int i = 0;
    while (m_xmlMarkUp.FindChildElem(_T("IED")))
    {
        m_xmlMarkUp.IntoElem();
        wstring sIEDName = m_xmlMarkUp.GetAttrib(_T("name"));

        m_iedmap[sIEDName];
        IEDMap::iterator Iedit = m_iedmap.find(sIEDName);

        wstring attributeName, attributeValue;
        for (int index = 0; ; index++)
        {
            wstring attributeName = m_xmlMarkUp.GetAttribName(index);
            if (attributeName.size() != 0)
            {
                attributeValue = m_xmlMarkUp.GetAttrib(attributeName);
                Iedit->second.mapAttributes[attributeName] = attributeValue;
            }
            else
                break;
        }

        while(m_xmlMarkUp.FindChildElem(L"Private"))
        {
            m_xmlMarkUp.IntoElem();
            
            wstring sType = m_xmlMarkUp.GetAttrib(L"type");
            if(sType == L"IED virtual terminal conection CRC")
            {
                CString sCrcInFile = m_xmlMarkUp.GetData().c_str();
                sCrcInFile.Trim();
                Iedit->second.sCrcInFile = sCrcInFile.GetBuffer(0);
            }
            m_xmlMarkUp.OutOfElem();
        }
        m_xmlMarkUp.ResetChildPos();
        while (m_xmlMarkUp.FindChildElem(_T("AccessPoint")))
        {
            m_xmlMarkUp.IntoElem();
            while (m_xmlMarkUp.FindChildElem(_T("Server")))
            {
                m_xmlMarkUp.IntoElem();
                while(m_xmlMarkUp.FindChildElem(_T("LDevice")))
                {
                    m_xmlMarkUp.IntoElem();
                    wstring sLDName = m_xmlMarkUp.GetAttrib(_T("inst"));
                    bool bGse = false;
                    bool bSmv = false; // false表示未发现该类型的控制块
                    map<wstring,wstring> mapLN;

                    while (m_xmlMarkUp.FindChildElem(_T("LN0")))
                    {
                        m_xmlMarkUp.IntoElem();
                        wstring sLnType = m_xmlMarkUp.GetAttrib(_T("lnType"));
                        wstring sLnClass = m_xmlMarkUp.GetAttrib(_T("lnClass"));
                        wstring sLnInst = m_xmlMarkUp.GetAttrib(_T("inst"));
                        // 建立 lnClass+lnInst -> lnType的map 
                        mapLN[sLnClass+sLnInst] = sLnType;
                        // DataSet
                        DataSetMap dataSetMap;
                        m_xmlMarkUp.ResetChildPos();
                        while (m_xmlMarkUp.FindChildElem(_T("DataSet")))
                        {
                            m_xmlMarkUp.IntoElem();
                            wstring sDataSet = m_xmlMarkUp.GetAttrib(_T("name"));
                            FCDASeq vecFCDA;
                            while (m_xmlMarkUp.FindChildElem(_T("FCDA")))
                            {
                                m_xmlMarkUp.IntoElem();
                                FCDA fcda;
                                fcda.bPrefix = FALSE;
                                fcda.bDaName = FALSE;

                                for (int index = 0; ; index++)
                                {
                                    wstring attributeName = m_xmlMarkUp.GetAttribName(index);
                                    if (attributeName.size() != 0)
                                    {
                                        attributeValue = m_xmlMarkUp.GetAttrib(attributeName);
                                        fcda.mapAttributes[attributeName] = attributeValue;

                                        if(attributeName == L"prefix")
                                        {
                                            fcda.bPrefix = TRUE;
                                        }

                                        if(attributeName == L"daName")
                                        {
                                            fcda.bDaName = TRUE;
                                        }
                                    }
                                    else
                                        break;
                                }
                                //// 记录下数据集中引用的LnClass类型
                                //m_LnClassmap.insert(std::make_pair(fcda.sLnClass, fcda.sLnClass));
                                vecFCDA.push_back(fcda);
                                m_xmlMarkUp.OutOfElem();
                            }
                            dataSetMap.insert(std::make_pair(sDataSet, vecFCDA));
                            m_xmlMarkUp.OutOfElem();
                        }
                        m_xmlMarkUp.ResetChildPos();
                        map<wstring, wstring> mapGC;
                        while (m_xmlMarkUp.FindChildElem(_T("GSEControl")))
                        {
                            bGse = true;
                            m_xmlMarkUp.IntoElem();

                            AttributesMap mapCBAttributes;
                            for (int index = 0; ; index++)
                            {
                                wstring attributeName = m_xmlMarkUp.GetAttribName(index);
                                if (attributeName.size() != 0)
                                {
                                    attributeValue = m_xmlMarkUp.GetAttrib(attributeName);
                                    mapCBAttributes[attributeName] = attributeValue;
                                }
                                else
                                    break;
                            }

                            if (mapGC.find(mapCBAttributes[_T("appID")]) != mapGC.end())
                            {
                                m_xmlMarkUp.OutOfElem();
                                continue;
                            }
                            mapGC[mapCBAttributes[_T("appID")]] = mapCBAttributes[_T("appID")];


                            CString sCbCtrlString;
                            sCbCtrlString += _T("<GSEControl ");
                            AttributesMap::iterator itCB = mapCBAttributes.begin();
                            for (; itCB != mapCBAttributes.end(); itCB++)
                            {
                                // 去掉描述，CDC中不需要
                                if (itCB->first == L"desc")
                                {
                                    continue;
                                }
                                sCbCtrlString += L" ";
                                sCbCtrlString += CString(itCB->first.c_str());
                                sCbCtrlString += _T("=\"");
                                sCbCtrlString += CString(itCB->second.c_str()) + _T("\"");
                            }
                            sCbCtrlString += _T("/>");
                            sCbCtrlString.Replace(L"type=\"\"",L"");

                            wstring sPrivateInCB;
                            while(m_xmlMarkUp.FindChildElem(L"Private"))
                            {
                                m_xmlMarkUp.IntoElem();
                                sPrivateInCB += m_xmlMarkUp.GetSubDoc();
                                m_xmlMarkUp.OutOfElem();
                            }
                            wstring sCbPath = sIEDName + sLDName + _T("/LLN0$GO$") + mapCBAttributes[L"name"];
                            DataSetMap::iterator datSetIt = dataSetMap.find(mapCBAttributes[_T("datSet")]);
                            if (Iedit != m_iedmap.end())
                            {
                                OutCBMap::iterator it1 = Iedit->second.mapOutcb.find(sCbPath);
                                if(it1 == Iedit->second.mapOutcb.end())
                                {
                                    Iedit->second.mapOutcb[sCbPath];
                                    it1 = Iedit->second.mapOutcb.find(sCbPath);
                                }
                                if (it1 != Iedit->second.mapOutcb.end())
                                {
                                    Iedit->second.vecCBKey.push_back(sCbPath);

                                    it1->second.sPrivateInCB = sPrivateInCB;
                                    it1->second.sCbCtrlString = sCbCtrlString;
                                    it1->second.sDataSet  = mapCBAttributes[_T("datSet")];
                                    it1->second.sCbType = L"GSE";

                                    if (datSetIt != dataSetMap.end())
                                    {
                                        for (int i = 0; i < datSetIt->second.size(); i ++)
                                        {
                                            FCDA fcdatemp = datSetIt->second.at(i);

                                            wstring sldInst, sPrefix,slnClass, slnInst, sdoName, sdaName;
                                            if (fcdatemp.mapAttributes.find(L"ldInst") != fcdatemp.mapAttributes.end())
                                            {
                                                sldInst = fcdatemp.mapAttributes[L"ldInst"];
                                            }
                                            if (fcdatemp.mapAttributes.find(L"prefix") != fcdatemp.mapAttributes.end())
                                            {
                                                sPrefix = fcdatemp.mapAttributes[L"prefix"];
                                            }
                                            if (fcdatemp.mapAttributes.find(L"lnClass") != fcdatemp.mapAttributes.end())
                                            {
                                                slnClass = fcdatemp.mapAttributes[L"lnClass"];
                                            }
                                            if (fcdatemp.mapAttributes.find(L"lnInst") != fcdatemp.mapAttributes.end())
                                            {
                                                slnInst = fcdatemp.mapAttributes[L"lnInst"];
                                            }
                                            if (fcdatemp.mapAttributes.find(L"doName") != fcdatemp.mapAttributes.end())
                                            {
                                                sdoName = fcdatemp.mapAttributes[L"doName"];
                                            }
                                            if (fcdatemp.mapAttributes.find(L"daName") != fcdatemp.mapAttributes.end()){
                                                sdaName = fcdatemp.mapAttributes[L"daName"];
                                            }

                                            wstring sKey = sldInst + _T("/") + sPrefix + 
                                                slnClass + slnInst + _T(".") + sdoName;
                                            if (!sdaName.empty())
                                            {
                                                sKey += _T(".") + sdaName;
                                            }
                                            it1->second.mapFcda.insert(std::make_pair(sKey,fcdatemp));
                                            it1->second.vecFcdaKey.push_back(sKey);

                                            Iedit->second.mapSigCB.insert(make_pair(sIEDName+_T("/")+sKey,sCbPath));
                                        }
                                    }
                                }
                            }
                            m_xmlMarkUp.OutOfElem();
                        }
                        m_xmlMarkUp.ResetChildPos();

                        while (m_xmlMarkUp.FindChildElem(_T("SampledValueControl")))
                        {
                            bSmv = true;
                            m_xmlMarkUp.IntoElem();

                            AttributesMap mapCBAttributes;
                            for (int index = 0; ; index++)
                            {
                                wstring attributeName = m_xmlMarkUp.GetAttribName(index);
                                if (attributeName.size() != 0)
                                {
                                    attributeValue = m_xmlMarkUp.GetAttrib(attributeName);
                                    mapCBAttributes[attributeName] = attributeValue;
                                }
                                else
                                    break;
                            }
                            CString sCbCtrlString;
                            sCbCtrlString += _T("<SampledValueControl ");
                            AttributesMap::iterator itCB = mapCBAttributes.begin();
                            for (; itCB != mapCBAttributes.end(); itCB++)
                            {
                                if (itCB->first == L"desc")
                                {
                                    continue;
                                }
                                sCbCtrlString += L" " + CString(itCB->first.c_str()) + L"=\"" + CString(itCB->second.c_str()) + L"\"";
                            }
                            sCbCtrlString += _T(">");
                            sCbCtrlString.Replace(L"multicast=\"\"",L"");

                            m_xmlMarkUp.ResetChildPos();
                            CString sSmvOpts;
                            if(m_xmlMarkUp.FindChildElem(L"SmvOpts"))
                            {
                                m_xmlMarkUp.IntoElem();
                                AttributesMap mapCBAttributes;
                                for (int index = 0; ; index++)
                                {
                                    wstring attributeName = m_xmlMarkUp.GetAttribName(index);
                                    if (attributeName.size() != 0)
                                    {
                                        attributeValue = m_xmlMarkUp.GetAttrib(attributeName);
                                        mapCBAttributes[attributeName] = attributeValue;
                                    }
                                    else
                                        break;
                                }

                                sCbCtrlString += _T("<SmvOpts ");
                                AttributesMap::iterator itCB = mapCBAttributes.begin();
                                for (; itCB != mapCBAttributes.end(); itCB++)
                                {
                                    sCbCtrlString += _T(" ") + CString(itCB->first.c_str()) + _T("=\"") + CString(itCB->second.c_str()) + _T("\"");
                                }
                                sCbCtrlString += _T("/>");
                                sSmvOpts.Replace(L"dataRef=\"\"",L"");
                                sSmvOpts.Replace(L"refreshTime=\"\"",L"");
                                sSmvOpts.Replace(L"sampleRate=\"\"",L"");
                                sSmvOpts.Replace(L"sampleSynchronized=\"\"",L"");
                                sSmvOpts.Replace(L"security=\"\"",L"");
                                m_xmlMarkUp.OutOfElem();

                                sCbCtrlString.Append(sSmvOpts);
                                sCbCtrlString += L"\n</SampledValueControl>";
                            }
                            else
                            {
                                sCbCtrlString.Replace(L">",L"/>");
                            }
                            wstring sPrivateInCB;
                            m_xmlMarkUp.ResetChildPos();
                            while(m_xmlMarkUp.FindChildElem(L"Private"))
                            {
                                m_xmlMarkUp.IntoElem();
                                sPrivateInCB += m_xmlMarkUp.GetSubDoc();
                                m_xmlMarkUp.OutOfElem();
                            }
                            wstring sCbPath = sIEDName + sLDName + _T("/LLN0$SV$") + mapCBAttributes[L"name"];
                            DataSetMap::iterator datSetIt = dataSetMap.find(mapCBAttributes[L"datSet"]);
                            if (datSetIt != dataSetMap.end() && Iedit != m_iedmap.end())
                            {
                                OutCBMap::iterator it1 = Iedit->second.mapOutcb.find(sCbPath);
                                if(it1 == Iedit->second.mapOutcb.end())
                                {
                                    Iedit->second.mapOutcb[sCbPath];

                                    it1 = Iedit->second.mapOutcb.find(sCbPath);
                                }
                                if (it1 != Iedit->second.mapOutcb.end())
                                {
                                    Iedit->second.vecCBKey.push_back(sCbPath);
                                    it1->second.sPrivateInCB = sPrivateInCB;
                                    it1->second.sCbCtrlString = sCbCtrlString;
                                    it1->second.sDataSet  = mapCBAttributes[L"datSet"];
                                    it1->second.sCbType = L"SMV";

                                    for (int i = 0; i < datSetIt->second.size(); i ++)
                                    {
                                        FCDA fcdatemp = datSetIt->second.at(i);
                                        wstring sldInst, sPrefix,slnClass, slnInst, sdoName, sdaName;
                                        if (fcdatemp.mapAttributes.find(L"ldInst") != fcdatemp.mapAttributes.end())
                                        {
                                            sldInst = fcdatemp.mapAttributes[L"ldInst"];
                                        }
                                        if (fcdatemp.mapAttributes.find(L"prefix") != fcdatemp.mapAttributes.end())
                                        {
                                            sPrefix = fcdatemp.mapAttributes[L"prefix"];
                                        }
                                        if (fcdatemp.mapAttributes.find(L"lnClass") != fcdatemp.mapAttributes.end())
                                        {
                                            slnClass = fcdatemp.mapAttributes[L"lnClass"];
                                        }
                                        if (fcdatemp.mapAttributes.find(L"lnInst") != fcdatemp.mapAttributes.end())
                                        {
                                            slnInst = fcdatemp.mapAttributes[L"lnInst"];
                                        }
                                        if (fcdatemp.mapAttributes.find(L"doName") != fcdatemp.mapAttributes.end())
                                        {
                                            sdoName = fcdatemp.mapAttributes[L"doName"];
                                        }
                                        if (fcdatemp.mapAttributes.find(L"daName") != fcdatemp.mapAttributes.end()){
                                            sdaName = fcdatemp.mapAttributes[L"daName"];
                                        }
                                        // wstring sKey = fcdatemp.sLdInst + _T("/") + fcdatemp.sPrefix + fcdatemp.sLnClass + fcdatemp.sLnInst + _T(".") + fcdatemp.sDoName;
                                        wstring sKey = sldInst + _T("/") + sPrefix + slnClass + slnInst + _T(".") + sdoName;

                                        if (!sdaName.empty())
                                        {
                                            sKey += _T(".") + sdaName;
                                        }
                                        it1->second.mapFcda.insert(std::make_pair(sKey,fcdatemp));
                                        it1->second.vecFcdaKey.push_back(sKey);

                                        Iedit->second.mapSigCB.insert(make_pair(sIEDName+_T("/")+sKey,sCbPath));
                                    }
                                }
                            }
                            m_xmlMarkUp.OutOfElem();
                        }

                        m_xmlMarkUp.ResetChildPos();
                        while (m_xmlMarkUp.FindChildElem(_T("Inputs")))
                        {
                            m_xmlMarkUp.IntoElem();
                            while (m_xmlMarkUp.FindChildElem(_T("ExtRef")))
                            {
                                m_xmlMarkUp.IntoElem();
                                wstring sExIed = m_xmlMarkUp.GetAttrib(_T("iedName"));
                                wstring sExLdInst = m_xmlMarkUp.GetAttrib(_T("ldInst"));
                                wstring sExPrefix = m_xmlMarkUp.GetAttrib(_T("prefix"));
                                wstring sExLnClass = m_xmlMarkUp.GetAttrib(_T("lnClass"));
                                wstring sExLnInst = m_xmlMarkUp.GetAttrib(_T("lnInst"));
                                wstring sExDoName = m_xmlMarkUp.GetAttrib(_T("doName"));
                                wstring sExDaName = m_xmlMarkUp.GetAttrib(_T("daName"));
                                wstring sIntAddr = m_xmlMarkUp.GetAttrib(_T("intAddr"));

                                wstring sExPath = sExIed + _T("/") + sExLdInst + _T("/") + sExPrefix + sExLnClass + sExLnInst + _T(".") + sExDoName;
                                if (!sExDaName.empty())
                                {
                                    sExPath += _T(".") + sExDaName;
                                }
                                wstring sInpath = sIntAddr;

                                if(Iedit != m_iedmap.end())
                                {
                                    Input input;
                                    input.sInAddr = sInpath;
                                    input.sOutIED = sExIed;
                                    input.sOutPath = sExPath;

                                    wstring realIntAddr = sIntAddr;
                                    if (sIntAddr.find(L":") != wstring::npos)
                                    {
                                        realIntAddr = sIntAddr.substr(sIntAddr.find(L":") + 1);
                                    }

                                    // Iedit->second.mapInsignal.insert(make_pair(sInpath,input));
                                    // Iedit->second.vecInsignal.push_back(sInpath);
                                    Iedit->second.vecInsignal.push_back(realIntAddr);
                                    Iedit->second.mapInsignal.insert(make_pair(realIntAddr,input));
                                    Iedit->second.mapOutIn.insert(make_pair(sExPath,sInpath));
                                }
                                m_xmlMarkUp.OutOfElem();
                            }
                            m_xmlMarkUp.OutOfElem();
                        }

                        m_xmlMarkUp.ResetChildPos();
                        // 建立 ldInst/prefix+lnclass+lnInst.doname.sdiname.sdaname -> DAI 的map
                        wstring sKey = sLDName+_T("/") + sLnClass + sLnInst;
                        TravellDOI(Iedit->second, sKey);

                        m_xmlMarkUp.OutOfElem();
                    }
                    /*
                    查找本端设备控制块中输出信号的bType
                    a、根据FCDA的lnClass和lnInst定位LNType
                    b、根据LN的lnType 和 FCDA的doName 定位DO
                    c、根据DO的type 和 FCDA的daName 定位BDA的type
                    */
                    /*
                    遍历LN节点
                    1、建立 lnClass+lnInst -> lnType的map 
                    2、建立 ldInst/prefix+lnclass+lnInst.doname.sdiname.sdaname -> DAI 的map
                    */
                    while (m_xmlMarkUp.FindChildElem(_T("LN")))
                    {
                        m_xmlMarkUp.IntoElem();
                        wstring sLnType = m_xmlMarkUp.GetAttrib(_T("lnType"));
                        wstring sLnClass = m_xmlMarkUp.GetAttrib(_T("lnClass"));
                        wstring sLnInst = m_xmlMarkUp.GetAttrib(_T("inst"));
                        wstring sPrefix = m_xmlMarkUp.GetAttrib(_T("prefix"));
                        // 1、建立 lnClass+lnInst -> lnType的map 
                        mapLN[sPrefix+sLnClass+sLnInst] = sLnType;
                        // 建立 ldInst/prefix+lnclass+lnInst.doname.sdiname.sdaname -> DAI 的map
                        wstring sKey = sLDName+_T("/")+sPrefix+sLnClass+sLnInst;
                        TravellDOI(Iedit->second, sKey);

                        m_xmlMarkUp.OutOfElem();
                    }
                    OutCBMap& mapCB = Iedit->second.mapOutcb;
                    OutCBMap::iterator itCB = mapCB.begin();
                    while(itCB!=mapCB.end())
                    {
                        FCDAMap& mapFcda = itCB->second.mapFcda;
                        FCDAMap::iterator itFcda = mapFcda.begin();
                        while(itFcda!=mapFcda.end())
                        {
                            // a、根据FCDA的lnClass和lnInst定位LNType
                            FCDA& fd = itFcda->second;
                            wstring sldInst, sPrefix,slnClass, slnInst, sdoName, sdaName;
                            if (fd.mapAttributes.find(L"ldInst") != fd.mapAttributes.end())
                            {
                                sldInst = fd.mapAttributes[L"ldInst"];
                            }
                            if (fd.mapAttributes.find(L"prefix") != fd.mapAttributes.end())
                            {
                                sPrefix = fd.mapAttributes[L"prefix"];
                            }
                            if (fd.mapAttributes.find(L"lnClass") != fd.mapAttributes.end())
                            {
                                slnClass = fd.mapAttributes[L"lnClass"];
                            }
                            if (fd.mapAttributes.find(L"lnInst") != fd.mapAttributes.end())
                            {
                                slnInst = fd.mapAttributes[L"lnInst"];
                            }
                            if (fd.mapAttributes.find(L"doName") != fd.mapAttributes.end())
                            {
                                sdoName = fd.mapAttributes[L"doName"];
                            }
                            if (fd.mapAttributes.find(L"daName") != fd.mapAttributes.end()){
                                sdaName = fd.mapAttributes[L"daName"];
                            }

                            wstring sLnType = mapLN[sPrefix + slnClass + slnInst];
                            // b、根据LN的lnType 和 FCDA的doName 定位DO
                            DOMap& mapDo = m_LNodeTypeMap[sLnType];
                            TypeCache tc;
                            tc.flage = TypeCache::IN_DO;
                            tc.mapDo = mapDo;
                            GetFcdaBType(split_string(sdoName + L"." + sdaName, L".", true), tc, fd.sBType);

                            itFcda++;
                        }
                        itCB++;
                    }
                    m_xmlMarkUp.OutOfElem();
                }
                m_xmlMarkUp.OutOfElem();
            }
            m_xmlMarkUp.OutOfElem();
        }
        m_xmlMarkUp.OutOfElem();
    }
    return true;
}

void CSCDCRC::GetFcdaBType(vector<wstring> vecName, TypeCache& tc, wstring& sBType)
{
    if (vecName.size() == 0)
    {
        return;
    }
    wstring sName = vecName[0];

    TypeCache tcInner;
    if (tc.flage == TypeCache::IN_DO)
    {
        DOMap::iterator itDo = tc.mapDo.find(sName);
        DAMap::iterator itDa = tc.mapDa.find(sName);
        if (itDo != tc.mapDo.end())
        {
            wDOType& doType = m_DOTypeMap[itDo->second.sType];
            if (!doType.sdoMap.empty())
            {
                tcInner.flage = TypeCache::IN_DO;
                tcInner.mapDo = doType.sdoMap;
            }
            else if(!doType.daMap.empty())
            {
                tcInner.flage = TypeCache::IN_DA;
                tcInner.mapDa = doType.daMap;
            }
            return GetFcdaBType(vector<wstring>(vecName.begin()+1, vecName.end()),tcInner, sBType);
        }
        else if(itDa != tc.mapDa.end())
        {
            BDAMap& daType = m_DATypeMap[itDa->second.sType];
            tcInner.flage = TypeCache::IN_DA;
            tcInner.mapBda = daType;
            return GetFcdaBType(vector<wstring>(vecName.begin()+1, vecName.end()),tcInner, sBType);
        }
    }
    else if(tc.flage == TypeCache::IN_DA)
    {
        DAMap::iterator itDA = tc.mapDa.find(sName);
        if (itDA != tc.mapDa.end())
        {
            wDA& da = itDA->second;

            if (!da.sType.empty() && (m_DATypeMap.find(da.sType) != m_DATypeMap.end()))
            {
                tcInner.flage = TypeCache::IN_BDA;
                tcInner.mapBda = m_DATypeMap[da.sType];
                return GetFcdaBType(vector<wstring>(vecName.begin()+1, vecName.end()),tcInner, sBType);
            }
            else
            {
                sBType = da.sbType;
                return;
            }
        }
    }
    else
    {
        BDAMap::iterator itBDA = tc.mapBda.find(sName);
        if (itBDA != tc.mapBda.end())
        {
            wBDA& bda = itBDA->second;

            if (!bda.sType.empty() && (m_DATypeMap.find(bda.sType) != m_DATypeMap.end()))
            {
                tcInner.flage = TypeCache::IN_BDA;
                tcInner.mapBda = m_DATypeMap[bda.sType];
                return GetFcdaBType(vector<wstring>(vecName.begin()+1, vecName.end()),tcInner, sBType);
            }
            else
            {
                sBType = bda.sbType;
                return;
            }
        }
    }
}
/**
* @brief    GetDAIPath简易说明
* 
* 获取DAI的路径
*
* @param    sPath   返回的路径
* @param    sDoType 当前的DoType
* @param    dateref  参数返回
* @return   布尔值
* @author   修黎明 
* @date     2013/09/1
*/
bool CSCDCRC::GetDAIPath(wstring& sPath, wstring sDoType, DataRef& dataref)
{
    if (m_xmlMarkUp.GetTagName() == _T("DAI"))
    {
        wstring sdaName = m_xmlMarkUp.GetAttrib(_T("name"));
        DOTypeMap::iterator dotypeIt = m_DOTypeMap.find(sDoType);
        if (dotypeIt != m_DOTypeMap.end())
        {
            DAMap::iterator daIt = dotypeIt->second.daMap.find(sdaName);
            if (daIt != dotypeIt->second.daMap.end())
            {
                dataref.sBType = daIt->second.sbType;
            }
        }
        sPath += _T(".") + sdaName;
        dataref.sDAIorDOI = m_xmlMarkUp.GetSubDoc();
    }
    else if (m_xmlMarkUp.GetTagName() == _T("SDI"))
    {
        wstring sSDOName =  m_xmlMarkUp.GetAttrib(_T("name"));
        sPath += _T(".") + sSDOName;

        DOTypeMap::iterator dotypeIt = m_DOTypeMap.find(sDoType);
        if (dotypeIt != m_DOTypeMap.end())
        {
            DOMap::iterator sdoIt = dotypeIt->second.sdoMap.find(sSDOName);
            if (sdoIt != dotypeIt->second.sdoMap.end())
            {
                sDoType = sdoIt->second.sType;
            }
        }

        while (m_xmlMarkUp.FindChildElem(_T("SDI")))
        {
            m_xmlMarkUp.IntoElem();
            GetDAIPath(sPath, sDoType, dataref);
            m_xmlMarkUp.OutOfElem();
        }
        while (m_xmlMarkUp.FindChildElem(_T("DAI")))
        {
            m_xmlMarkUp.IntoElem();
            GetDAIPath(sPath, sDoType, dataref);
            m_xmlMarkUp.OutOfElem();
        }
    }
    return true;
}
/**
* @brief    DataTypeTemplateImport简易说明
* 
* 解析DataTypeTemplateImport部分
*
* @return   bool    布尔值
* @author   修黎明 
* @date     2013/09/1
*/
bool CSCDCRC::DataTypeTemplateImport()
{
    log_info(_T("执行 DataTypeTemplateImport"));
    // 该部分用于求取bType值
    while (m_xmlMarkUp.FindChildElem(_T("DataTypeTemplates")))
    {
        m_xmlMarkUp.IntoElem();
        while (m_xmlMarkUp.FindChildElem(_T("LNodeType")))
        {
            m_xmlMarkUp.IntoElem();

            wstring sLNodeTypeId = m_xmlMarkUp.GetAttrib(_T("id"));
            DOMap doMap;
            while (m_xmlMarkUp.FindChildElem(_T("DO")))
            {
                m_xmlMarkUp.IntoElem();
                wDO dostr;
                dostr.sName = m_xmlMarkUp.GetAttrib(_T("name"));
                dostr.sType = m_xmlMarkUp.GetAttrib(_T("type"));
                doMap.insert(std::make_pair(dostr.sName, dostr));
                m_xmlMarkUp.OutOfElem();
            }
            m_LNodeTypeMap.insert(std::make_pair(sLNodeTypeId, doMap));
            m_xmlMarkUp.OutOfElem();
        }
        while (m_xmlMarkUp.FindChildElem(_T("DOType")))
        {
            m_xmlMarkUp.IntoElem();
            wDOType dotype;
            wstring sDOTypeId = m_xmlMarkUp.GetAttrib(_T("id"));
            while (m_xmlMarkUp.FindChildElem(_T("DA")))
            {
                m_xmlMarkUp.IntoElem();
                wDA da;
                da.sName = m_xmlMarkUp.GetAttrib(L"name");
                da.sbType = m_xmlMarkUp.GetAttrib(L"bType");
                da.sType = m_xmlMarkUp.GetAttrib(L"type");
                dotype.daMap.insert(std::make_pair(da.sName, da));
                m_xmlMarkUp.OutOfElem();
            }
            while (m_xmlMarkUp.FindChildElem(L"SDO"))
            {
                m_xmlMarkUp.IntoElem();
                wDO sdo;
                sdo.sName = m_xmlMarkUp.GetAttrib(L"name");
                sdo.sType = m_xmlMarkUp.GetAttrib(L"type");
                dotype.sdoMap.insert(std::make_pair(sdo.sName, sdo));
                m_xmlMarkUp.OutOfElem();
            }
            m_DOTypeMap.insert(std::make_pair(sDOTypeId, dotype));
            m_xmlMarkUp.OutOfElem();
        }
        while (m_xmlMarkUp.FindChildElem(L"DAType"))
        {
            m_xmlMarkUp.IntoElem();
            wstring sDATypeId = m_xmlMarkUp.GetAttrib(L"id");
            BDAMap bdaMap;
            while (m_xmlMarkUp.FindChildElem(L"BDA"))
            {
                m_xmlMarkUp.IntoElem();
                wBDA bda;
                bda.sName = m_xmlMarkUp.GetAttrib(L"name");
                bda.sbType = m_xmlMarkUp.GetAttrib(L"bType");
                bda.sType = m_xmlMarkUp.GetAttrib(L"type");
                bdaMap.insert(std::make_pair(bda.sName, bda));
                m_xmlMarkUp.OutOfElem();
            }
            m_DATypeMap.insert(std::make_pair(sDATypeId, bdaMap));
            m_xmlMarkUp.OutOfElem();
        }
        m_xmlMarkUp.OutOfElem();
    }
    return true;
}

/**
* @brief    WriteXml简易说明
* 
* 写XML
*
* @return   bool    布尔值
* @author   修黎明 
* @date     2013/09/01
*/
bool CSCDCRC::WriteXml()
{
    USES_CONVERSION;
    // SCD的校验码
    wstring     m_strSCD;

    for (IEDMap::iterator it = m_IEDMap.begin(); it != m_IEDMap.end(); it ++)
    {
        CMarkup Scdxml;
        Scdxml.AddElem(_T("IED"));
        Scdxml.SetAttrib(_T("name"), it->first);
        BOOL bHavePubOrSub = FALSE;

        Scdxml.IntoElem();
        /////////// GOOSEPUB //////////////
        BOOL bHasControl = FALSE;
        for(UINT iCB=0;iCB<it->second.vecCBKey.size();iCB++)
        {
            OutCBMap::iterator outcbIt = it->second.mapOutcb.find(it->second.vecCBKey[iCB]);
            CString sCBType = outcbIt->second.sCbType.c_str();
            if(sCBType!=_T("GSE")) continue;

            if(!bHasControl) 
            {
                bHasControl = TRUE;
                Scdxml.AddElem(_T("GOOSEPUB"));
                Scdxml.IntoElem();
            }
            bHavePubOrSub = TRUE;
            Scdxml.AddElem(_T("GOCBref"));
            Scdxml.AddAttrib(_T("name"),outcbIt->first);

            CString sCbCtrlString = CString(_T("\n"))+outcbIt->second.sCbCtrlString.c_str()+CString(_T("\n"));
            CString sPrivateInCB = outcbIt->second.sPrivateInCB.c_str();
            if(!sPrivateInCB.IsEmpty())
            {
                sCbCtrlString.Replace(L"/>",L">\n");
                sCbCtrlString.Append(sPrivateInCB);
                sCbCtrlString.Append(L"</GSEControl>\n");
            }

            //modify by cxl 2014-09-17 18:45
            CString sConnectedAPString;
            if(outcbIt->second.bConnectedAp && outcbIt->second.sCbString.empty()==false)
            {
                sConnectedAPString = CString(_T("\n")) + outcbIt->second.sApConnectedAPString.c_str() + CString(_T("\n"));
                sConnectedAPString.Replace(L"/>",L">\n");
                sConnectedAPString.Append(outcbIt->second.sApPrivateString.c_str());
                sConnectedAPString+= CString(_T("\n")) + outcbIt->second.sCbString.c_str();
                sConnectedAPString+= CString(_T("\n")) + outcbIt->second.sPhysConnString.c_str() + CString(_T("\n"));
                sConnectedAPString.Append(L"</ConnectedAP>\n");
            }
            sCbCtrlString += sConnectedAPString;
            Scdxml.SetElemContent(sCbCtrlString.GetBuffer(0));
            //end 

            Scdxml.IntoElem();
            if (outcbIt->second.vecFcdaKey.size() > 0)
            {
                Scdxml.AddElem(_T("DataSet"));
                Scdxml.AddAttrib(_T("name"), outcbIt->second.sDataSet);

                CString sFCDAStrings;
                for(UINT iFcda=0;iFcda < outcbIt->second.vecFcdaKey.size();iFcda++)
                {
                    FCDAMap::iterator fcdaIt = outcbIt->second.mapFcda.find(outcbIt->second.vecFcdaKey[iFcda]);

                    if (fcdaIt != outcbIt->second.mapFcda.end())
                    {
                        CString sFCDA;
                        sFCDA += _T("<FCDA ");
                        AttributesMap::iterator itFCDAAtr = fcdaIt->second.mapAttributes.begin();
                        for (; itFCDAAtr != fcdaIt->second.mapAttributes.end(); itFCDAAtr++)
                        {
                            sFCDA += L" " + CString(itFCDAAtr->first.c_str()) + L"=\"" + CString(itFCDAAtr->second.c_str()) + L"\"";
                        }
                        if(!fcdaIt->second.sBType.empty())
                             sFCDA += L" " + CString(L"bType") + L"=\"" + CString(fcdaIt->second.sBType.c_str()) + L"\"";
                        sFCDA += _T("/>");

                        wstring sDai = it->second.mapSignalDai[fcdaIt->first];
                        if(!sDai.empty()) 
                        {
                            sFCDA.Replace(L"/>",L">\n");
                            sFCDA += sDai.c_str();
                            sFCDA += _T("\n</FCDA>\n");
                        }
                        sFCDAStrings += sFCDA;
                    }
                }
                Scdxml.SetElemContent(sFCDAStrings.GetBuffer(0));
            }

            Scdxml.OutOfElem();
        }
        if(bHasControl) 
        {
            Scdxml.OutOfElem();
            bHasControl = FALSE;
        }
        /////////// GOOSESUB //////////////
        //缓存已经出现过的控制块
        map<wstring,wstring> mapCBPath;
        for(UINT iInput=0;iInput<it->second.vecInsignal.size();iInput++)
        {
            //对端信号路径
            wstring sInPath = it->second.vecInsignal[iInput];
            //本端信号路径
            wstring sExtPath = it->second.mapInsignal[sInPath].sOutPath;
            wstring sExtIed  = it->second.mapInsignal[sInPath].sOutIED;

            IEDMap::iterator itIED = m_IEDMap.find(sExtIed);
            if(itIED == m_IEDMap.end()) continue;

            //对端设备 根据信号路径获得控制块
            wstring sCBPath = itIED->second.mapSigCB[sExtPath];

            OutCBMap::iterator outcbIt = itIED->second.mapOutcb.find(sCBPath);
            if(outcbIt == itIED->second.mapOutcb.end()) continue;

            if(mapCBPath.find(sCBPath)!=mapCBPath.end()) continue;
            mapCBPath[sCBPath] = sCBPath;

            if(outcbIt->second.sCbType != _T("GSE")) continue;

            if(!bHasControl)
            {
                bHasControl = TRUE;
                Scdxml.AddElem(_T("GOOSESUB"));
                Scdxml.IntoElem();
            }
            bHavePubOrSub = TRUE;
            Scdxml.AddElem(_T("GOCBref"));
            Scdxml.AddAttrib(_T("name"), outcbIt->first);
            CString sCbCtrlString = CString(_T("\n")) + outcbIt->second.sCbCtrlString.c_str() + CString(_T("\n"));

            CString sConnectedAPString;
            if(outcbIt->second.bConnectedAp  && outcbIt->second.sCbString.empty()==false)
            {
                sConnectedAPString = outcbIt->second.sApConnectedAPString.c_str();
                sConnectedAPString.Replace(L"/>",L">\n");
                sConnectedAPString += CString(_T("\n")) + outcbIt->second.sCbString.c_str() + CString(_T("\n"));
                sConnectedAPString.Append(L"</ConnectedAP>\n");
            }
            sCbCtrlString += sConnectedAPString;
            Scdxml.SetElemContent(sCbCtrlString.GetBuffer(0));

            Scdxml.IntoElem();
            vector<wstring>& vecFcdaKey = outcbIt->second.vecFcdaKey;
            if (vecFcdaKey.size() > 0)
            {
                Scdxml.AddElem(_T("DataSet"));
                Scdxml.AddAttrib(_T("name"), outcbIt->second.sDataSet);
                Scdxml.IntoElem();

                FCDAMap& mapFcda = outcbIt->second.mapFcda;
                //输出输入连接
                VirLinkOutIn& mapOutIn = it->second.mapOutIn;

                for(UINT iFcda=0;iFcda<vecFcdaKey.size();iFcda++)
                {
                    FCDAMap::iterator fcdaIt = mapFcda.find(vecFcdaKey[iFcda]);
                    //拼接路径                
                    wstring sKey = itIED->first + _T("/") + fcdaIt->first;
                    //根据路径查找有无关联信号(可能多个)
                    bool bFcdaHasSignal = false;
                    wstring sIntAddr;

                    pair<VirLinkIt,VirLinkIt> insignals = mapOutIn.equal_range(sKey);

                    Scdxml.AddElem(_T("FCDA"));
                    Scdxml.AddAttrib(L"bType",fcdaIt->second.sBType);
                    Scdxml.IntoElem();

                    BOOL bIntAddr = FALSE;

                    for(VirLinkIt itVirLink=insignals.first;itVirLink!=insignals.second;itVirLink++)
                    {
                        bIntAddr = TRUE;

                        sIntAddr = itVirLink->second;
                        if(sIntAddr.empty()) continue;

                        Scdxml.AddElem(_T("intAddr"));
                        Scdxml.AddAttrib(_T("name"),sIntAddr);

                        // 将sIntAddr中的物理端口去掉
                        wstring realIntAddr = sIntAddr;
                        if (sIntAddr.find(L":") != wstring::npos)
                        {
                            realIntAddr = sIntAddr.substr(sIntAddr.find(L":") + 1);
                        }

                        wstring sIntAddrDAI = it->second.mapSignalDai[realIntAddr];
                        if(!sIntAddrDAI.empty()) 
                        {
                            Scdxml.SetElemContent(wstring(_T("\n"))+sIntAddrDAI);
                        }
                    }

                    if(!bIntAddr)
                    {
                        Scdxml.AddElem(_T("intAddr"));
                        Scdxml.AddAttrib(_T("name"),_T("NULL"));
                    }

                    Scdxml.OutOfElem();
                }
                Scdxml.OutOfElem();
            }
            Scdxml.OutOfElem();
        }
        if(bHasControl)
        {
            Scdxml.OutOfElem();
            bHasControl = FALSE;
        }
        /////////// SVPUB //////////////
        bHasControl = FALSE;    
        for(UINT iCB=0;iCB<it->second.vecCBKey.size();iCB++)        
        {
            OutCBMap::iterator outcbIt = it->second.mapOutcb.find(it->second.vecCBKey[iCB]);
            CString sCBType = outcbIt->second.sCbType.c_str();
            if(sCBType!=_T("SMV")) continue;

            if(!bHasControl) 
            {
                bHasControl = TRUE;
                Scdxml.AddElem(_T("SVPUB"));
                Scdxml.IntoElem();
            }
            bHavePubOrSub = TRUE;
            Scdxml.AddElem(_T("SMVCBref"));
            wstring bref = outcbIt->first;
            bref.replace(outcbIt->first.find_first_of(L"$") + 1, 2, L"MS");
            Scdxml.AddAttrib(_T("name"),bref);

            CString sCbCtrlString = CString(_T("\n"))+outcbIt->second.sCbCtrlString.c_str()+CString(_T("\n"));
            CString sPrivateInCB = outcbIt->second.sPrivateInCB.c_str();
            if(!sPrivateInCB.IsEmpty())
            {
                sCbCtrlString.Replace(L"/>",L">\n");
                sCbCtrlString.Append(sPrivateInCB);
                sCbCtrlString.Append(L"</SampledValueControl>\n");
            }
            // Scdxml.SetElemContent(sCbCtrlString.GetBuffer(0));

            //modify by cxl 2014-09-17 18:45
            CString sConnectedAPString;
            if(outcbIt->second.bConnectedAp  && outcbIt->second.sCbString.empty()==false)
            {
                sConnectedAPString = CString(_T("\n")) + outcbIt->second.sApConnectedAPString.c_str() + CString(_T("\n"));
                sConnectedAPString.Replace(L"/>",L">\n");
                sConnectedAPString.Append(outcbIt->second.sApPrivateString.c_str());
                sConnectedAPString+= CString(_T("\n")) + outcbIt->second.sCbString.c_str();
                sConnectedAPString+= CString(_T("\n")) + outcbIt->second.sPhysConnString.c_str() + CString(_T("\n"));
                sConnectedAPString.Append(L"</ConnectedAP>\n");
            }
            sCbCtrlString += sConnectedAPString;
            Scdxml.SetElemContent(sCbCtrlString.GetBuffer(0));

            Scdxml.IntoElem();
            if (outcbIt->second.vecFcdaKey.size() > 0)
            {
                Scdxml.AddElem(_T("DataSet"));
                Scdxml.AddAttrib(_T("name"), outcbIt->second.sDataSet);
                CString sFCDAStrings;
                for(UINT iFcda=0;iFcda < outcbIt->second.vecFcdaKey.size();iFcda++)
                {
                    FCDAMap::iterator fcdaIt = outcbIt->second.mapFcda.find(outcbIt->second.vecFcdaKey[iFcda]);

                    if (fcdaIt != outcbIt->second.mapFcda.end())
                    {
                        CString sFCDA;
                        sFCDA += _T("<FCDA ");
                        AttributesMap::iterator itFCDAAtr = fcdaIt->second.mapAttributes.begin();
                        for (; itFCDAAtr != fcdaIt->second.mapAttributes.end(); itFCDAAtr++)
                        {
                            sFCDA += L" " + CString(itFCDAAtr->first.c_str()) + L"=\"" + CString(itFCDAAtr->second.c_str()) + L"\"";
                        }
                        if(!fcdaIt->second.sBType.empty())
                            sFCDA += L" " + CString(L"bType") + L"=\"" + CString(fcdaIt->second.sBType.c_str()) + L"\"";
                        sFCDA += _T("/>");

                        wstring sDai = it->second.mapSignalDai[fcdaIt->first];
                        if(!sDai.empty()) 
                        {
                            sFCDA.Replace(L"/>",L">\n");
                            sFCDA += sDai.c_str();
                            sFCDA += _T("\n</FCDA>\n");
                        }
                        sFCDAStrings += sFCDA;
                    }
                }
                Scdxml.SetElemContent(sFCDAStrings.GetBuffer(0));
            }
            Scdxml.OutOfElem();
        }
        if(bHasControl) 
        {
            Scdxml.OutOfElem();
            bHasControl = FALSE;
        }
        /////////// SVSUB //////////////
        //缓存已经出现过的控制块
        mapCBPath.clear();
        for(UINT iInput=0;iInput<it->second.vecInsignal.size();iInput++)
        {
            //对端信号路径
            wstring sInPath = it->second.vecInsignal[iInput];
            //本端信号路径
            wstring sExtPath = it->second.mapInsignal[sInPath].sOutPath;
            wstring sExtIed  = it->second.mapInsignal[sInPath].sOutIED;

            IEDMap::iterator itIED = m_IEDMap.find(sExtIed);
            if(itIED == m_IEDMap.end()) continue;

            //对端设备 根据信号路径获得控制块
            wstring sCBPath = itIED->second.mapSigCB[sExtPath];
            OutCBMap::iterator outcbIt = itIED->second.mapOutcb.find(sCBPath);
            if(outcbIt == itIED->second.mapOutcb.end()) continue;

            if(mapCBPath.find(sCBPath)!=mapCBPath.end()) continue;
            mapCBPath[sCBPath] = sCBPath;

            if(outcbIt->second.sCbType != _T("SMV")) continue;

            if(!bHasControl)
            {
                bHasControl = TRUE;
                Scdxml.AddElem(_T("SVSUB"));
                Scdxml.IntoElem();
            }
            bHavePubOrSub = TRUE;
            Scdxml.AddElem(_T("SMVCBref"));
            wstring bref = outcbIt->first;
            bref.replace(outcbIt->first.find_first_of(L"$") + 1, 2, L"MS");
            Scdxml.AddAttrib(_T("name"),bref);

            CString sCbCtrlString = CString(_T("\n")) + outcbIt->second.sCbCtrlString.c_str() + CString(_T("\n"));

            CString sConnectedAPString;
            if(outcbIt->second.bConnectedAp  && outcbIt->second.sCbString.empty()==false)
            {
                sConnectedAPString = outcbIt->second.sApConnectedAPString.c_str();
                sConnectedAPString.Replace(L"/>",L">\n");
                sConnectedAPString += CString(_T("\n")) + outcbIt->second.sCbString.c_str() + CString(_T("\n"));
                sConnectedAPString.Append(L"</ConnectedAP>\n");
            }
            sCbCtrlString += sConnectedAPString;
            Scdxml.SetElemContent(sCbCtrlString.GetBuffer(0));

            Scdxml.IntoElem();
            vector<wstring>& vecFcdaKey = outcbIt->second.vecFcdaKey;
            if (vecFcdaKey.size() > 0)
            {
                Scdxml.AddElem(_T("DataSet"));
                Scdxml.AddAttrib(_T("name"), outcbIt->second.sDataSet);
                Scdxml.IntoElem();

                FCDAMap& mapFcda = outcbIt->second.mapFcda;
                //输出输入连接
                VirLinkOutIn& mapOutIn = it->second.mapOutIn;

                for(UINT iFcda=0;iFcda<vecFcdaKey.size();iFcda++)
                {
                    FCDAMap::iterator fcdaIt = mapFcda.find(vecFcdaKey[iFcda]);
                    //拼接路径
                    wstring sKey = itIED->first + _T("/") + fcdaIt->first;
                    //根据路径查找有无关联信号(可能多个)
                    bool bFcdaHasSignal = false;
                    wstring sIntAddr;

                    pair<VirLinkIt,VirLinkIt> insignals = mapOutIn.equal_range(sKey);

                    Scdxml.AddElem(_T("FCDA"));
                    if (fcdaIt->second.sBType.size() > 0)
                    {
                        Scdxml.AddAttrib(L"bType",fcdaIt->second.sBType);
                    }
                    Scdxml.IntoElem();

                    BOOL bIntAddr = FALSE;

                    for(VirLinkIt itVirLink=insignals.first;itVirLink!=insignals.second;itVirLink++)
                    {
                        bIntAddr = TRUE;

                        sIntAddr = itVirLink->second;

                        if(sIntAddr.empty()) continue;
                        Scdxml.AddElem(_T("intAddr"));
                        Scdxml.AddAttrib(_T("name"),sIntAddr);

                        wstring realIntAddr = sIntAddr;
                        if (sIntAddr.find(L":") != wstring::npos)
                        {
                            realIntAddr = sIntAddr.substr(sIntAddr.find(L":") + 1);
                        }
                        wstring sIntAddrDAI = it->second.mapSignalDai[realIntAddr];
                        if(!sIntAddrDAI.empty()) 
                        {
                            Scdxml.SetElemContent(wstring(_T("\n"))+sIntAddrDAI);
                        }
                    }
                    if(!bIntAddr)
                    {
                        Scdxml.AddElem(_T("intAddr"));
                        Scdxml.AddAttrib(_T("name"),_T("NULL"));
                    }

                    Scdxml.OutOfElem();
                }
                Scdxml.OutOfElem();
            }

            Scdxml.OutOfElem();
        }
        if(bHasControl)
        {
            Scdxml.OutOfElem();
            bHasControl = FALSE;
        }
        Scdxml.ResetMainPos();

        bool bCrc = true;
        CString sHeader;
        if (bCrc)
        {
            sHeader = _T("<?xmlversion=\"1.0\"encoding=\"UTF-8\"?>");
        }
        else
        {
            sHeader = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
        }

        CString sXml = sHeader;
        CString sFormat;
        if (bCrc)
        {
            sFormat.Format(_T("<IEDname=\"%s\">"),it->first.c_str());
        }
        else
        {
            sFormat.Format(_T("<IED name=\"%s\">\n"),it->first.c_str());
        }
        sXml += sFormat;
        TravellXml(Scdxml, sXml, bCrc);
        sXml += _T("</IED>");

        if (bHavePubOrSub == TRUE)
        {
        //     HMODULE hDll = NULL;
        //     GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)&TravellXml, &hDll);
        //     // 保存文件
        //     TCHAR szPath[_MAX_PATH] = {0};
        //     GetModuleFileName(hDll, szPath, _MAX_PATH );
        //     PathRemoveFileSpec(szPath);
        //     PathAppend(szPath, m_sScd.c_str());
        //     SHCreateDirectory(NULL, szPath);
        //     PathAppend(szPath, (it->first + _T(".txt")).c_str());
        //     Scdxml.SetDoc((LPCTSTR)sXml);
        //     Scdxml.Save(szPath);

            char szCrc[10]={0};

            string xml = WtoU(sXml.GetString());
            m_objCrc.generate_crc(xml.c_str(), xml.size(),szCrc);
            // m_objCrc.generate_crc(( unsigned char*)(const char*)sCrcTxt.GetBuffer(0),sCrcTxt.GetLength(),szCrc);

            wstring sCrc = AtoW(szCrc);
            // IED的CRC码
            wCrcCode crcCode;
            crcCode.sIedName = it->first;
            crcCode.sIedDesc = it->second.mapAttributes[L"desc"];
            crcCode.sIedManu = it->second.mapAttributes[L"manufacturer"];
            crcCode.sIedType = it->second.mapAttributes[L"type"];
            crcCode.sCrc = sCrc;
            crcCode.sCrcInFile = it->second.sCrcInFile;
            m_IedCrcCodeVec.push_back(crcCode);
            m_IedCrcCodeMap[it->first] = crcCode;
        }

    }
    // m_IEDMap.clear(); //用完后清空列表 

    return true;
}

bool CSCDCRC::WriteCCD()
{
    USES_CONVERSION;
    // SCD的校验码
    wstring     m_strSCD;

    //for (IEDMap::iterator it = m_IEDMap.begin(); it != m_IEDMap.end(); it++)
    //{
    //    wstring ied = it->first;
    //    CMarkup Scdxml;
    //    Scdxml.AddElem(_T("IED"));
    //    for (AttributesMap::iterator itIEDAttr = it->second.mapAttributes.begin(); itIEDAttr != it->second.mapAttributes.end(); itIEDAttr++)
    //    {
    //        Scdxml.SetAttrib(itIEDAttr->first, itIEDAttr->second);
    //    }
    //    
    //    BOOL bHavePubOrSub = FALSE;

    //    Scdxml.IntoElem();
    //    /////////// GOOSEPUB //////////////
    //    BOOL bHasControl = FALSE;
    //    for(UINT iCB=0;iCB<it->second.vecCBKey.size();iCB++)
    //    {
    //        OutCBMap::iterator outcbIt = it->second.mapOutcb.find(it->second.vecCBKey[iCB]);
    //        CString sCBType = outcbIt->second.sCbType.c_str();
    //        if(sCBType!=_T("GSE")) continue;

    //        if(!bHasControl) 
    //        {
    //            bHasControl = TRUE;
    //            Scdxml.AddElem(_T("GOOSEPUB"));
    //            Scdxml.IntoElem();
    //        }
    //        bHavePubOrSub = TRUE;
    //        Scdxml.AddElem(_T("GOCBref"));
    //        Scdxml.AddAttrib(_T("name"),outcbIt->first);

    //        CString sCbCtrlString = CString(_T("\n"))+outcbIt->second.sCbCtrlString.c_str()+CString(_T("\n"));
    //        CString sPrivateInCB = outcbIt->second.sPrivateInCB.c_str();
    //        if(!sPrivateInCB.IsEmpty())
    //        {
    //            sCbCtrlString.Replace(L"/>",L">\n");
    //            sCbCtrlString.Append(sPrivateInCB);
    //            sCbCtrlString.Append(L"</GSEControl>\n");
    //        }

    //        //modify by cxl 2014-09-17 18:45
    //        CString sConnectedAPString;
    //        if(outcbIt->second.bConnectedAp && outcbIt->second.sCbString.empty()==false)
    //        {
    //            sConnectedAPString = CString(_T("\n")) + outcbIt->second.sApConnectedAPString.c_str() + CString(_T("\n"));
    //            sConnectedAPString.Replace(L"/>",L">\n");
    //            sConnectedAPString.Append(outcbIt->second.sApPrivateString.c_str());
    //            sConnectedAPString+= CString(_T("\n")) + outcbIt->second.sCbString.c_str();
    //            sConnectedAPString+= CString(_T("\n")) + outcbIt->second.sPhysConnString.c_str() + CString(_T("\n"));
    //            sConnectedAPString.Append(L"</ConnectedAP>\n");
    //        }
    //        sCbCtrlString += sConnectedAPString;
    //        Scdxml.SetElemContent(sCbCtrlString.GetBuffer(0));
    //        //end 

    //        Scdxml.IntoElem();
    //        if (outcbIt->second.vecFcdaKey.size() > 0)
    //        {
    //            Scdxml.AddElem(_T("DataSet"));
    //            Scdxml.AddAttrib(_T("name"), outcbIt->second.sDataSet);

    //            CString sFCDAStrings;
    //            for(UINT iFcda=0;iFcda < outcbIt->second.vecFcdaKey.size();iFcda++)
    //            {
    //                FCDAMap::iterator fcdaIt = outcbIt->second.mapFcda.find(outcbIt->second.vecFcdaKey[iFcda]);

    //                if (fcdaIt != outcbIt->second.mapFcda.end())
    //                {
    //                    CString sFCDA;
    //                    sFCDA += _T("<FCDA ");
    //                    AttributesMap::iterator itFCDAAtr = fcdaIt->second.mapAttributes.begin();
    //                    for (; itFCDAAtr != fcdaIt->second.mapAttributes.end(); itFCDAAtr++)
    //                    {
    //                        sFCDA += L" " + CString(itFCDAAtr->first.c_str()) + L"=\"" + CString(itFCDAAtr->second.c_str()) + L"\"";
    //                    }
    //                    if(!fcdaIt->second.sBType.empty())
    //                        sFCDA += L" " + CString(L"bType") + L"=\"" + CString(fcdaIt->second.sBType.c_str()) + L"\"";

    //                    wstring doKey = fcdaIt->first;
    //                    std::vector<wstring> list = split_string(fcdaIt->first, L".", true);
    //                    int i = 2;
    //                    while (list.size() >= i)
    //                    {
    //                        map<wstring, wstring>::iterator itDesc = it->second.mapSignalDaiDesc.find(doKey);
    //                        if (itDesc != it->second.mapSignalDaiDesc.end() && !itDesc->second.empty())
    //                        {
    //                            sFCDA += L" " + CString(L"desc") + L"=\"" + CString(itDesc->second.c_str()) + L"\"";
    //                            break;
    //                        }
    //                        else
    //                        {
    //                            doKey = doKey.substr(0, doKey.find_last_of(L"."));
    //                            i++;
    //                        }
    //                    }
    //                    //wstring sDaiDesc = it->second.mapSignalDaiDesc[doKey];
    //                    //if(!sDaiDesc.empty()) 
    //                    //    sFCDA += L" " + CString(L"desc") + L"=\"" + CString(sDaiDesc.c_str()) + L"\"";
    //                    sFCDA += _T("/>");

    //                    wstring sDai = it->second.mapSignalDai[fcdaIt->first];
    //                    if(!sDai.empty()) 
    //                    {
    //                        sFCDA.Replace(L"/>",L">\n");
    //                        sFCDA += sDai.c_str();
    //                        sFCDA += _T("\n</FCDA>\n");
    //                    }
    //                    sFCDAStrings += sFCDA;
    //                }
    //            }
    //            Scdxml.SetElemContent(sFCDAStrings.GetBuffer(0));
    //        }

    //        Scdxml.OutOfElem();
    //    }
    //    if(bHasControl) 
    //    {
    //        Scdxml.OutOfElem();
    //        bHasControl = FALSE;
    //    }
    //    /////////// GOOSESUB //////////////
    //    //缓存已经出现过的控制块
    //    map<wstring,wstring> mapCBPath;
    //    for(UINT iInput=0;iInput<it->second.vecInsignal.size();iInput++)
    //    {
    //        //对端信号路径
    //        wstring sInPath = it->second.vecInsignal[iInput];
    //        //本端信号路径
    //        wstring sExtPath = it->second.mapInsignal[sInPath].sOutPath;
    //        wstring sExtIed  = it->second.mapInsignal[sInPath].sOutIED;

    //        IEDMap::iterator itIED = m_IEDMap.find(sExtIed);
    //        if(itIED == m_IEDMap.end()) continue;

    //        //对端设备 根据信号路径获得控制块
    //        wstring sCBPath = itIED->second.mapSigCB[sExtPath];

    //        OutCBMap::iterator outcbIt = itIED->second.mapOutcb.find(sCBPath);
    //        if(outcbIt == itIED->second.mapOutcb.end()) continue;

    //        if(mapCBPath.find(sCBPath)!=mapCBPath.end()) continue;
    //        mapCBPath[sCBPath] = sCBPath;

    //        if(outcbIt->second.sCbType != _T("GSE")) continue;

    //        if(!bHasControl)
    //        {
    //            bHasControl = TRUE;
    //            Scdxml.AddElem(_T("GOOSESUB"));
    //            Scdxml.IntoElem();
    //        }
    //        bHavePubOrSub = TRUE;
    //        Scdxml.AddElem(_T("GOCBref"));
    //        Scdxml.AddAttrib(_T("name"), outcbIt->first);
    //        CString sCbCtrlString = CString(_T("\n")) + outcbIt->second.sCbCtrlString.c_str() + CString(_T("\n"));

    //        CString sConnectedAPString;
    //        if(outcbIt->second.bConnectedAp  && outcbIt->second.sCbString.empty()==false)
    //        {
    //            sConnectedAPString = outcbIt->second.sApConnectedAPString.c_str();
    //            sConnectedAPString.Replace(L"/>",L">\n");
    //            sConnectedAPString += CString(_T("\n")) + outcbIt->second.sCbString.c_str() + CString(_T("\n"));
    //            sConnectedAPString.Append(L"</ConnectedAP>\n");
    //        }
    //        sCbCtrlString += sConnectedAPString;
    //        Scdxml.SetElemContent(sCbCtrlString.GetBuffer(0));

    //        Scdxml.IntoElem();
    //        vector<wstring>& vecFcdaKey = outcbIt->second.vecFcdaKey;
    //        if (vecFcdaKey.size() > 0)
    //        {
    //            Scdxml.AddElem(_T("DataSet"));
    //            Scdxml.AddAttrib(_T("name"), outcbIt->second.sDataSet);
    //            Scdxml.IntoElem();

    //            FCDAMap& mapFcda = outcbIt->second.mapFcda;
    //            //输出输入连接
    //            VirLinkOutIn& mapOutIn = it->second.mapOutIn;

    //            for(UINT iFcda=0;iFcda<vecFcdaKey.size();iFcda++)
    //            {
    //                FCDAMap::iterator fcdaIt = mapFcda.find(vecFcdaKey[iFcda]);
    //                //拼接路径                
    //                wstring sKey = itIED->first + _T("/") + fcdaIt->first;
    //                //根据路径查找有无关联信号(可能多个)
    //                bool bFcdaHasSignal = false;
    //                wstring sIntAddr;

    //                pair<VirLinkIt,VirLinkIt> insignals = mapOutIn.equal_range(sKey);

    //                Scdxml.AddElem(_T("FCDA"));
    //                AttributesMap::iterator itFCDAAtr = fcdaIt->second.mapAttributes.begin();
    //                for (; itFCDAAtr != fcdaIt->second.mapAttributes.end(); itFCDAAtr++)
    //                {
    //                    Scdxml.AddAttrib(itFCDAAtr->first, itFCDAAtr->second);
    //                }

    //                wstring doKey = fcdaIt->first;
    //                std::vector<wstring> list = split_string(fcdaIt->first, L".", true);
    //                int i = 2;
    //                if (L"RPIT/CKYLTC1.TapChg.valWTr.posVal" == doKey)
    //                {
    //                    OutputDebugString(L"DOKey");
    //                }
    //                while (list.size() >= i)
    //                {
    //                    map<wstring, wstring>::iterator itDesc = itIED->second.mapSignalDaiDesc.find(doKey);
    //                    if (itDesc != itIED->second.mapSignalDaiDesc.end() && !itDesc->second.empty())
    //                    {
    //                        Scdxml.AddAttrib(L"desc",itDesc->second);
    //                        break;
    //                    }
    //                    else
    //                    {
    //                        doKey = doKey.substr(0, doKey.find_last_of(L"."));
    //                        i++;
    //                    }
    //                }

    //                Scdxml.AddAttrib(L"bType",fcdaIt->second.sBType);
    //                Scdxml.IntoElem();

    //                BOOL bIntAddr = FALSE;

    //                for(VirLinkIt itVirLink=insignals.first;itVirLink!=insignals.second;itVirLink++)
    //                {
    //                    bIntAddr = TRUE;

    //                    sIntAddr = itVirLink->second;
    //                    if(sIntAddr.empty()) continue;

    //                    Scdxml.AddElem(_T("intAddr"));
    //                    Scdxml.AddAttrib(_T("name"),sIntAddr);

    //                    // 将sIntAddr中的物理端口去掉
    //                    wstring realIntAddr = sIntAddr;
    //                    if (sIntAddr.find(L":") != wstring::npos)
    //                    {
    //                        realIntAddr = sIntAddr.substr(sIntAddr.find(L":") + 1);
    //                    }
    //                    wstring doKey = realIntAddr;
    //                    std::vector<wstring> list = split_string(realIntAddr, L".", true);
    //                    int i = 2;
    //                    while (list.size() >= i)
    //                    {
    //                        map<wstring, wstring>::iterator itDesc = it->second.mapSignalDaiDesc.find(doKey);
    //                        if (itDesc != it->second.mapSignalDaiDesc.end() && !itDesc->second.empty())
    //                        {
    //                            Scdxml.AddAttrib(L"desc",itDesc->second);
    //                            break;
    //                        }
    //                        else
    //                        {
    //                            doKey = doKey.substr(0, doKey.find_last_of(L"."));
    //                            i++;
    //                        }
    //                    }
    //                    //wstring sDaiDesc = it->second.mapSignalDaiDesc[doKey];
    //                    //if(!sDaiDesc.empty()) 
    //                    //    Scdxml.AddAttrib(L"desc",sDaiDesc);

    //                    wstring sIntAddrDAI = it->second.mapSignalDai[realIntAddr];
    //                    if(!sIntAddrDAI.empty()) 
    //                    {
    //                        Scdxml.SetElemContent(wstring(_T("\n"))+sIntAddrDAI);
    //                    }
    //                }

    //                if(!bIntAddr)
    //                {
    //                    Scdxml.AddElem(_T("intAddr"));
    //                    Scdxml.AddAttrib(_T("desc"),_T(""));
    //                    Scdxml.AddAttrib(_T("name"),_T("NULL"));
    //                }

    //                Scdxml.OutOfElem();
    //            }
    //            Scdxml.OutOfElem();
    //        }
    //        Scdxml.OutOfElem();
    //    }
    //    if(bHasControl)
    //    {
    //        Scdxml.OutOfElem();
    //        bHasControl = FALSE;
    //    }
    //    /////////// SVPUB //////////////
    //    bHasControl = FALSE;    
    //    for(UINT iCB=0;iCB<it->second.vecCBKey.size();iCB++)        
    //    {
    //        OutCBMap::iterator outcbIt = it->second.mapOutcb.find(it->second.vecCBKey[iCB]);
    //        CString sCBType = outcbIt->second.sCbType.c_str();
    //        if(sCBType!=_T("SMV")) continue;

    //        if(!bHasControl) 
    //        {
    //            bHasControl = TRUE;
    //            Scdxml.AddElem(_T("SVPUB"));
    //            Scdxml.IntoElem();
    //        }
    //        bHavePubOrSub = TRUE;
    //        Scdxml.AddElem(_T("SMVCBref"));
    //        wstring bref = outcbIt->first;
    //        bref.replace(outcbIt->first.find_first_of(L"$") + 1, 2, L"MS");
    //        Scdxml.AddAttrib(_T("name"),bref);

    //        CString sCbCtrlString = CString(_T("\n"))+outcbIt->second.sCbCtrlString.c_str()+CString(_T("\n"));
    //        CString sPrivateInCB = outcbIt->second.sPrivateInCB.c_str();
    //        if(!sPrivateInCB.IsEmpty())
    //        {
    //            sCbCtrlString.Replace(L"/>",L">\n");
    //            sCbCtrlString.Append(sPrivateInCB);
    //            sCbCtrlString.Append(L"</SampledValueControl>\n");
    //        }
    //        // Scdxml.SetElemContent(sCbCtrlString.GetBuffer(0));

    //        //modify by cxl 2014-09-17 18:45
    //        CString sConnectedAPString;
    //        if(outcbIt->second.bConnectedAp  && outcbIt->second.sCbString.empty()==false)
    //        {
    //            sConnectedAPString = CString(_T("\n")) + outcbIt->second.sApConnectedAPString.c_str() + CString(_T("\n"));
    //            sConnectedAPString.Replace(L"/>",L">\n");
    //            sConnectedAPString.Append(outcbIt->second.sApPrivateString.c_str());
    //            sConnectedAPString+= CString(_T("\n")) + outcbIt->second.sCbString.c_str();
    //            sConnectedAPString+= CString(_T("\n")) + outcbIt->second.sPhysConnString.c_str() + CString(_T("\n"));
    //            sConnectedAPString.Append(L"</ConnectedAP>\n");
    //        }
    //        sCbCtrlString += sConnectedAPString;
    //        Scdxml.SetElemContent(sCbCtrlString.GetBuffer(0));

    //        Scdxml.IntoElem();
    //        if (outcbIt->second.vecFcdaKey.size() > 0)
    //        {
    //            Scdxml.AddElem(_T("DataSet"));
    //            Scdxml.AddAttrib(_T("name"), outcbIt->second.sDataSet);
    //            CString sFCDAStrings;
    //            for(UINT iFcda=0;iFcda < outcbIt->second.vecFcdaKey.size();iFcda++)
    //            {
    //                FCDAMap::iterator fcdaIt = outcbIt->second.mapFcda.find(outcbIt->second.vecFcdaKey[iFcda]);

    //                if (fcdaIt != outcbIt->second.mapFcda.end())
    //                {
    //                    CString sFCDA;
    //                    sFCDA += _T("<FCDA ");
    //                    AttributesMap::iterator itFCDAAtr = fcdaIt->second.mapAttributes.begin();
    //                    for (; itFCDAAtr != fcdaIt->second.mapAttributes.end(); itFCDAAtr++)
    //                    {
    //                        sFCDA += L" " + CString(itFCDAAtr->first.c_str()) + L"=\"" + CString(itFCDAAtr->second.c_str()) + L"\"";
    //                    }
    //                    if(!fcdaIt->second.sBType.empty())
    //                        sFCDA += L" " + CString(L"bType") + L"=\"" + CString(fcdaIt->second.sBType.c_str()) + L"\"";

    //                    wstring doKey = fcdaIt->first;
    //                    std::vector<wstring> list = split_string(fcdaIt->first, L".", true);
    //                    int i = 2;
    //                    while (list.size() >= i)
    //                    {
    //                        map<wstring, wstring>::iterator itDesc = it->second.mapSignalDaiDesc.find(doKey);
    //                        if (itDesc != it->second.mapSignalDaiDesc.end() && !itDesc->second.empty())
    //                        {
    //                            sFCDA += L" " + CString(L"desc") + L"=\"" + CString(itDesc->second.c_str()) + L"\"";
    //                            break;
    //                        }
    //                        else
    //                        {
    //                            doKey = doKey.substr(0, doKey.find_last_of(L"."));
    //                            i++;
    //                        }
    //                    }
    //                    //wstring sDaiDesc = it->second.mapSignalDaiDesc[doKey];
    //                    //if(!sDaiDesc.empty()) 
    //                    //    sFCDA += L" " + CString(L"desc") + L"=\"" + CString(sDaiDesc.c_str()) + L"\"";
    //                    sFCDA += _T("/>");

    //                    wstring sDai = it->second.mapSignalDai[fcdaIt->first];
    //                    if(!sDai.empty()) 
    //                    {
    //                        sFCDA.Replace(L"/>",L">\n");
    //                        sFCDA += sDai.c_str();
    //                        sFCDA += _T("\n</FCDA>\n");
    //                    }
    //                    sFCDAStrings += sFCDA;
    //                }
    //            }
    //            Scdxml.SetElemContent(sFCDAStrings.GetBuffer(0));
    //        }
    //        Scdxml.OutOfElem();
    //    }
    //    if(bHasControl) 
    //    {
    //        Scdxml.OutOfElem();
    //        bHasControl = FALSE;
    //    }
    //    /////////// SVSUB //////////////
    //    //缓存已经出现过的控制块
    //    mapCBPath.clear();
    //    for(UINT iInput=0;iInput<it->second.vecInsignal.size();iInput++)
    //    {
    //        //对端信号路径
    //        wstring sInPath = it->second.vecInsignal[iInput];
    //        //本端信号路径
    //        wstring sExtPath = it->second.mapInsignal[sInPath].sOutPath;
    //        wstring sExtIed  = it->second.mapInsignal[sInPath].sOutIED;

    //        IEDMap::iterator itIED = m_IEDMap.find(sExtIed);
    //        if(itIED == m_IEDMap.end()) continue;

    //        //对端设备 根据信号路径获得控制块
    //        wstring sCBPath = itIED->second.mapSigCB[sExtPath];
    //        OutCBMap::iterator outcbIt = itIED->second.mapOutcb.find(sCBPath);
    //        if(outcbIt == itIED->second.mapOutcb.end()) continue;

    //        if(mapCBPath.find(sCBPath)!=mapCBPath.end()) continue;
    //        mapCBPath[sCBPath] = sCBPath;

    //        if(outcbIt->second.sCbType != _T("SMV")) continue;

    //        if(!bHasControl)
    //        {
    //            bHasControl = TRUE;
    //            Scdxml.AddElem(_T("SVSUB"));
    //            Scdxml.IntoElem();
    //        }
    //        bHavePubOrSub = TRUE;
    //        Scdxml.AddElem(_T("SMVCBref"));
    //        wstring bref = outcbIt->first;
    //        bref.replace(outcbIt->first.find_first_of(L"$") + 1, 2, L"MS");
    //        Scdxml.AddAttrib(_T("name"),bref);

    //        CString sCbCtrlString = CString(_T("\n")) + outcbIt->second.sCbCtrlString.c_str() + CString(_T("\n"));

    //        CString sConnectedAPString;
    //        if(outcbIt->second.bConnectedAp  && outcbIt->second.sCbString.empty()==false)
    //        {
    //            sConnectedAPString = outcbIt->second.sApConnectedAPString.c_str();
    //            sConnectedAPString.Replace(L"/>",L">\n");
    //            sConnectedAPString += CString(_T("\n")) + outcbIt->second.sCbString.c_str() + CString(_T("\n"));
    //            sConnectedAPString.Append(L"</ConnectedAP>\n");
    //        }
    //        sCbCtrlString += sConnectedAPString;
    //        Scdxml.SetElemContent(sCbCtrlString.GetBuffer(0));

    //        Scdxml.IntoElem();
    //        vector<wstring>& vecFcdaKey = outcbIt->second.vecFcdaKey;
    //        if (vecFcdaKey.size() > 0)
    //        {
    //            Scdxml.AddElem(_T("DataSet"));
    //            Scdxml.AddAttrib(_T("name"), outcbIt->second.sDataSet);
    //            Scdxml.IntoElem();

    //            FCDAMap& mapFcda = outcbIt->second.mapFcda;
    //            //输出输入连接
    //            VirLinkOutIn& mapOutIn = it->second.mapOutIn;

    //            for(UINT iFcda=0;iFcda<vecFcdaKey.size();iFcda++)
    //            {
    //                FCDAMap::iterator fcdaIt = mapFcda.find(vecFcdaKey[iFcda]);
    //                //拼接路径
    //                wstring sKey = itIED->first + _T("/") + fcdaIt->first;
    //                //根据路径查找有无关联信号(可能多个)
    //                bool bFcdaHasSignal = false;
    //                wstring sIntAddr;

    //                pair<VirLinkIt,VirLinkIt> insignals = mapOutIn.equal_range(sKey);

    //                Scdxml.AddElem(_T("FCDA"));
    //                AttributesMap::iterator itFCDAAtr = fcdaIt->second.mapAttributes.begin();
    //                for (; itFCDAAtr != fcdaIt->second.mapAttributes.end(); itFCDAAtr++)
    //                {
    //                    Scdxml.AddAttrib(itFCDAAtr->first, itFCDAAtr->second);
    //                }

    //                wstring doKey = fcdaIt->first;
    //                std::vector<wstring> list = split_string(fcdaIt->first, L".", true);
    //                int i = 2;
    //                while (list.size() >= i)
    //                {
    //                    map<wstring, wstring>::iterator itDesc = itIED->second.mapSignalDaiDesc.find(doKey);
    //                    if (itDesc != itIED->second.mapSignalDaiDesc.end() && !itDesc->second.empty())
    //                    {
    //                        Scdxml.AddAttrib(L"desc",itDesc->second);
    //                        break;
    //                    }
    //                    else
    //                    {
    //                        doKey = doKey.substr(0, doKey.find_last_of(L"."));
    //                        i++;
    //                    }
    //                }
    //                //wstring sDaiDesc = itIED->second.mapSignalDaiDesc[fcdaIt->first];
    //                //if(!sDaiDesc.empty()) 
    //                //    Scdxml.AddAttrib(L"desc",sDaiDesc);

    //                if (fcdaIt->second.sBType.size() > 0)
    //                {
    //                    Scdxml.AddAttrib(L"bType",fcdaIt->second.sBType);
    //                }
    //                Scdxml.IntoElem();

    //                BOOL bIntAddr = FALSE;

    //                for(VirLinkIt itVirLink=insignals.first;itVirLink!=insignals.second;itVirLink++)
    //                {
    //                    bIntAddr = TRUE;

    //                    sIntAddr = itVirLink->second;

    //                    if(sIntAddr.empty()) continue;
    //                    Scdxml.AddElem(_T("intAddr"));
    //                    Scdxml.AddAttrib(_T("name"),sIntAddr);

    //                    wstring realIntAddr = sIntAddr;
    //                    if (sIntAddr.find(L":") != wstring::npos)
    //                    {
    //                        realIntAddr = sIntAddr.substr(sIntAddr.find(L":") + 1);
    //                    }
    //                    wstring doKey = realIntAddr;
    //                    std::vector<wstring> list = split_string(realIntAddr, L".", true);
    //                    int i = 2;
    //                    while (list.size() >= i)
    //                    {
    //                        map<wstring, wstring>::iterator itDesc = it->second.mapSignalDaiDesc.find(doKey);
    //                        if (itDesc != it->second.mapSignalDaiDesc.end() && !itDesc->second.empty())
    //                        {
    //                            Scdxml.AddAttrib(L"desc",itDesc->second);
    //                            break;
    //                        }
    //                        else
    //                        {
    //                            doKey = doKey.substr(0, doKey.find_last_of(L"."));
    //                            i++;
    //                        }
    //                    }
    //                    //if (list.size() > 2)
    //                    //{
    //                    //    doKey = doKey.substr(0, doKey.find_last_of(L"."));
    //                    //}
    //                    //wstring sDaiDesc = it->second.mapSignalDaiDesc[doKey];
    //                    //if(!sDaiDesc.empty()) 
    //                    //    Scdxml.AddAttrib(L"desc",sDaiDesc);

    //                    wstring sIntAddrDAI = it->second.mapSignalDai[realIntAddr];
    //                    if(!sIntAddrDAI.empty()) 
    //                    {
    //                        Scdxml.SetElemContent(wstring(_T("\n"))+sIntAddrDAI);
    //                    }
    //                }
    //                if(!bIntAddr)
    //                {
    //                    Scdxml.AddElem(_T("intAddr"));
    //                    Scdxml.AddAttrib(_T("name"),_T("NULL"));
    //                    Scdxml.AddAttrib(_T("desc"),_T(""));
    //                }

    //                Scdxml.OutOfElem();
    //            }
    //            Scdxml.OutOfElem();
    //        }

    //        Scdxml.OutOfElem();
    //    }
    //    if(bHasControl)
    //    {
    //        Scdxml.OutOfElem();
    //        bHasControl = FALSE;
    //    }
    //    Scdxml.ResetMainPos();

    //    CString sHeader;
    //    sHeader = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");

    //    CString sXml = sHeader;
    //    CString sFormat = _T("<IED ");
    //    // sFormat.Format(_T("<IED name=\"%s\">\n"),it->first.c_str());
    //    for (AttributesMap::iterator itIEDAttr = it->second.mapAttributes.begin(); 
    //        itIEDAttr != it->second.mapAttributes.end(); itIEDAttr++)
    //    {
    //        sFormat += (itIEDAttr->first + L"=\"" + itIEDAttr->second + L"\"").c_str();
    //    }
    //    sFormat += L">\n";
    //    sXml += sFormat;
    //    TravellXml(Scdxml, sXml, false);
    //    // 增加时间戳和CRC
    //    CString sCRCCode = _T("<CRC ");
    //    CrcCodeMap::iterator itCRC = m_IedCrcCodeMap.find(it->first);
    //    if (itCRC != m_IedCrcCodeMap.end())
    //    {
    //        sCRCCode += _T("id=\"") + CString(m_IedCrcCodeMap[it->first].sCrc.c_str()) + "\" ";
    //    }
    //    
    //    CTime currentTime = CTime::GetCurrentTime();
    //    CString timeStamp;
    //    timeStamp.Format(L"%d-%d-%d %d:%d:%d", currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetDay(), currentTime.GetHour(), currentTime.GetMinute(), currentTime.GetSecond());
    //    sCRCCode += _T("timestamp=\"") + timeStamp + "\"/>\n";
    //    sXml += sCRCCode;
    //    sXml += _T("</IED>");


    //    if (bHavePubOrSub == TRUE)
    //    {
    //        HMODULE hDll = NULL;
    //        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)&TravellXml, &hDll);
    //        // 保存文件
    //        TCHAR szPath[_MAX_PATH] = {0};
    //        GetModuleFileName(hDll, szPath, _MAX_PATH );
    //        PathRemoveFileSpec(szPath);
    //        PathAppend(szPath, m_sScd.c_str());
    //        SHCreateDirectory(NULL, szPath);
    //        PathAppend(szPath, (it->first + _T(".ccd")).c_str());
    //        Scdxml.SetDoc((LPCTSTR)sXml);
    //        Scdxml.Save(szPath);
    //    }
    //}
    m_IEDMap.clear(); //用完后清空列表 

    return true;
}
