#include "../ScdCheckCommon/VtdTools.h"
#include "ConfigMgr.h"
#include "OutputMgr.h"
#include <fstream>
#include <set>
#include "../ScdCheckCommon/charconv.h"
#include <Windows.h>

using namespace std;
using namespace com_ximpleware;

CConfigMgr::CConfigMgr()
{
}

CConfigMgr::~CConfigMgr()
{
}

bool CConfigMgr::SetConfigFilePath(const char *lpFilePath)
{
    //
    m_configs.clear();

    //
    try
    {
        VTDGen vg;

        if (vg.parseFile(false, lpFilePath))
        {
            VTDNav *pVn = vg.getNav();

            if (pVn != NULL)
            {
                set<string> setItemNames;
                AutoPilot ap;

                ap.selectXPath(L"/SCLConfig/CheckClass/CheckItem[@key][@name]");
                ap.bind(pVn);

                while (ap.evalXPath() != -1)
                {
                    string strKey = WtoA(VtdGetAttributeValue(pVn, L"key"));

                    if (!strKey.empty())
                    {
                        if (m_configs.count(strKey) > 0)
                        {
                            throw std::runtime_error(strKey + "�������ļ����Ѵ���");
                        }

                        CheckItemConfig &cfg = m_configs[strKey];

                        cfg.strCheckItemKey = strKey;
                        cfg.strCheckItemName = WtoA(VtdGetAttributeValue(pVn, L"name"));
                        cfg.strCheckItemDesc = WtoA(VtdGetAttributeValue(pVn, L"desc"));
                        cfg.bEnabled = VtdGetAttributeValue(pVn, L"isCheck") == L"true";
                        cfg.strCheckItemRef = WtoA(VtdGetAttributeValue(pVn, L"standard"));

                        wstring strFlag = VtdGetAttributeValue(pVn, L"flag");

                        if (strFlag == L"W")
                        {
                            cfg.etValue = ET_WARNING;
                        }
                        else if (strFlag == L"E")
                        {
                            cfg.etValue = ET_ERROR;
                        }
                        else if (strFlag == L"R")
                        {
                            cfg.etValue = ET_TIP;
                        }
                        else
                        {
                            cfg.etValue = ET_ERROR;
                        }

                        // ���Ҵ�����Ϣ
                        {
                            pVn->push();
                            pVn->toElement(PARENT);
                            cfg.strCheckClassKey = WtoA(VtdGetAttributeValue(pVn, L"key"));
                            cfg.strCheckClassDesc = WtoA(VtdGetAttributeValue(pVn, L"desc"));
                            pVn->pop();
                        }

                        if (setItemNames.count(cfg.strCheckItemName) > 0)
                        {
                            throw std::runtime_error(cfg.strCheckItemName + "�������ļ����Ѵ���");
                        }
                        else
                        {
                            setItemNames.insert(cfg.strCheckItemName);
                        }
                    }
                }

                delete[] pVn->getXML();

                // ��������ʼ�ձ�������״̬
                map<string, CheckItemConfig>::iterator it;

                // xml��ʽ���ʼ��Ϊtrue
                CheckItemConfig &itXml = m_configs["CE18E6"];
                itXml.strCheckItemKey = "CE18E6";
                itXml.bEnabled = true;

                // һ�㾯����ʼ��Ϊtrue
                CheckItemConfig itWarn = m_configs["5E04BF"];
                itWarn.strCheckItemKey = "5E04BF";
                itWarn.bEnabled = true;

                // һ�������ʼ��Ϊtrue
                CheckItemConfig itError = m_configs["B861B0"];
                itError.strCheckItemKey = "B861B0";
                itError.bEnabled = true;

                return true;
            }
        }
        else
        {
            delete[] vg.getXML();
        }
    }
    catch (std::exception &e)
    {
        COutputMgr::GetInstance()->Error("SetConfigFilePath�쳣��%s\r\n", e.what());
    }

    return false;
}

CheckItemConfig::CheckItemConfig()
{
    bEnabled = false;
    etValue = ET_WARNING;
}

bool CConfigMgr::GenerateFiles() const
{
#ifdef _DEBUG
    //if (IsDebuggerPresent())
    {
        CreateDirectory(TEXT("CheckItems"), NULL);
        SetCurrentDirectory(TEXT("CheckItems"));

        for (map<string, CheckItemConfig>::const_iterator it = m_configs.begin(); it != m_configs.end(); ++it)
        {
            const CheckItemConfig &cfg = it->second;
            string strClassName = "C" + cfg.strCheckItemName;

            // .h
            try
            {
                ofstream f;

                f.open(cfg.strCheckItemName + ".h");

                f<<"/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���"<<endl;
                f<<" * "<<endl;
                f<<" * Key: "<<cfg.strCheckItemKey<<endl;
                f<<" * Name: "<<cfg.strCheckItemName<<endl;
                f<<" * Desc: "<<cfg.strCheckItemDesc<<endl;
                f<<" * Ref: "<<cfg.strCheckItemRef<<endl;
                f<<" */"<<endl;
                f<<endl;

                f<<"#ifndef _"<<cfg.strCheckItemName<<"_H"<<endl;
                f<<"#define _"<<cfg.strCheckItemName<<"_H"<<endl;
                f<<endl;

                f<<"#include \"../../ScdCheckCommon/Common.h\""<<endl;
                f<<"#include \"../CheckItem.h\""<<endl;
                f<<endl;

                f<<"class "<<strClassName<<" : public CSingleInstance<"<<strClassName<<">, CNoCopy, public CCheckItemBase"<<endl;
                f<<"{"<<endl;
                f<<"public:"<<endl;
                f<<"    "<<strClassName<<"();"<<endl;
                f<<endl;
                f<<"public:"<<endl;
                f<<"    virtual bool CheckInternal();"<<endl;
                f<<"};"<<endl;

                f<<endl;
                f<<"#endif /* _"<<cfg.strCheckItemName<<"_H */"<<endl;

                f.close();
            }
            catch (std::exception &ex)
            {
            }

            // .cpp
            try
            {
                ofstream f;

                f.open(cfg.strCheckItemName + ".cpp");

                f<<"#include \""<<cfg.strCheckItemName<<".h\""<<endl;
                f<<"#include \"../ScdChecker.h\""<<endl;
                f<<"#include \"../ConfigMgr.h\""<<endl;
                f<<"#include \"../CacheMgr.h\""<<endl;
                f<<"#include \"../OutputMgr.h\""<<endl;
                f<<"#include \"../../ScdCheckCommon/charconv.h\""<<endl;
                f<<"#include \"../../ScdCheckCommon/VtdTools.h\""<<endl;
                f<<"#include <assert.h>"<<endl;
                f<<endl;

                f<<"using namespace std;"<<endl;
                f<<"using namespace com_ximpleware;"<<endl;
                f<<endl;

                f<<"#define KEY \""<<cfg.strCheckItemKey<<"\""<<endl;
                f<<endl;

                f<<strClassName<<"::"<<strClassName<<"()"<<endl;
                f<<"{"<<endl;
                f<<"    CheckItemConfig cfg;"<<endl;
                f<<endl;
                f<<"    // ȡ����Ŀ��������Ϣ�����õ�������"<<endl;
                f<<"    if (CConfigMgr::GetInstance()->GetItemConfig(KEY, cfg))"<<endl;
                f<<"    {"<<endl;
                f<<"        SetConfig(cfg);"<<endl;
                f<<"    }"<<endl;
                f<<endl;
                f<<"    // ��������˱���Ŀ����������ע��"<<endl;
                f<<"    if (IsEnabled())"<<endl;
                f<<"    {"<<endl;
                f<<"        CScdChecker::GetInstance()->RegisterCheckItem(this);"<<endl;
                f<<endl;
                f<<"        // �ڴ˴������Ҫĳĳ���͵Ļ�����Ϣ�������Լ��������Ŀ"<<endl;
                f<<"        CCacheMgr::GetInstance()->AddNeededCacheMark(KEY);"<<endl;
                f<<"    }"<<endl;
                f<<"}"<<endl;
                f<<endl;

                f<<"bool "<<strClassName<<"::CheckInternal()"<<endl;
                f<<"{"<<endl;
                f<<"    // �����"<<endl;
                f<<"    CScdChecker *pChecker = CScdChecker::GetInstance();"<<endl;
                f<<"    // �����������"<<endl;
                f<<"    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();"<<endl;
                f<<"    // ���������"<<endl;
                f<<"    COutputMgr *pOutputMgr = COutputMgr::GetInstance();"<<endl;
                f<<"    // �����ļ�������"<<endl;
                f<<"    CConfigMgr *pConfigMgr = CConfigMgr::GetInstance();"<<endl;
                f<<endl;
                f<<"    assert(pChecker != NULL);"<<endl;
                f<<"    assert(pCacheMgr != NULL);"<<endl;
                f<<"    assert(pOutputMgr != NULL);"<<endl;
                f<<"    assert(pConfigMgr != NULL);"<<endl;
                f<<endl;
                f<<"    // custom code goes here"<<endl;
                f<<endl;
                f<<endl;
                f<<"    return true;"<<endl;
                f<<"}"<<endl;

                f.close();
            }
            catch (std::exception &ex)
            {
            }
        }

        // _InitAllCheckItems.cpp
        try
        {
            ofstream f;

            f.open("_InitAllCheckItems.cpp");

            f<<"#include \"_InitAllCheckItems.h\""<<endl;
            for (map<string, CheckItemConfig>::const_iterator it = m_configs.begin(); it != m_configs.end(); ++it)
            {
                f<<"#include \""<<it->second.strCheckItemName<<".h\""<<endl;
            }
            f<<endl;

            f<<"bool InitAllCheckItems()"<<endl;
            f<<"{"<<endl;
            f<<"    return true";
            for (map<string, CheckItemConfig>::const_iterator it = m_configs.begin(); it != m_configs.end(); ++it)
            {
                f<<" &&"<<endl<<"        C"<<it->second.strCheckItemName<<"::InitializeInstance()";
            }
            f<<";"<<endl;
            f<<"}"<<endl;

            f.close();
        }
        catch (std::exception &ex)
        {
        }
    }
#endif

    return true;
}

bool CConfigMgr::GetItemConfig(const std::string &strKey, CheckItemConfig &cfg) const
{
    map<string, CheckItemConfig>::const_iterator it = m_configs.find(strKey);

    if (it != m_configs.end())
    {
        cfg = it->second;
        return true;
    }

    return false;
}
