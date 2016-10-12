#include "LnValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

#include <Shlwapi.h>
#include <atlstr.h>
#include <fstream>
#include "..\..\ScdCheckCommon\Markup.h"

using namespace std;
using namespace com_ximpleware;

#define KEY "0D52B6"

CLnValidCheck::CLnValidCheck()
{
    CheckItemConfig cfg;

    // ȡ����Ŀ��������Ϣ�����õ�������
    if (CConfigMgr::GetInstance()->GetItemConfig(KEY, cfg))
    {
        SetConfig(cfg);
    }

    // ��������˱���Ŀ����������ע��
    if (IsEnabled())
    {
        CScdChecker::GetInstance()->RegisterCheckItem(this);

        // �ڴ˴������Ҫĳĳ���͵Ļ�����Ϣ�������Լ��������Ŀ
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetIedTemplateOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_IED");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
    }
}

bool CLnValidCheck::CheckInternal()
{
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    // ���������
    COutputMgr *pOutputMgr = COutputMgr::GetInstance();
    // �����ļ�������
    CConfigMgr *pConfigMgr = CConfigMgr::GetInstance();

    assert(pChecker != NULL);
    assert(pCacheMgr != NULL);
    assert(pOutputMgr != NULL);
    assert(pConfigMgr != NULL);

    // custom code goes here

    return true;
}

void CLnValidCheck::CheckValid_InCacheMgr(ICDCache& icd)
{
    CLnValidCheck* pLnValidCheck = CLnValidCheck::GetInstance();

    if (!pLnValidCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    CStringA sPath = pChecker->GetFilePathToCheck().c_str();
    sPath.Replace("/","\\");

    char szPath[MAX_PATH] = {0};
    lstrcpynA(szPath, (LPCTSTR)sPath, MAX_PATH);
    PathRemoveFileSpecA(szPath);
    PathAppendA(szPath, "icd_type.xml");

    if (!PathFileExistsA(szPath))
    {
        pLnValidCheck->Report(
            pChecker->CalcRowNumberFromOffset(0),
            pChecker->GetRowContentByOffset(0),
            "%s",
            "icd_type.xml�ļ������ڣ������޷����");
        return;
    }

    wstring sIcdName;
    wstring sIcdVoltage;

    CMarkup xml;
    xml.Load(szPath);
    while (xml.FindElem("Config"))
    {
        xml.IntoElem();
        if (xml.FindElem("ICD"))
        {
            sIcdName = AtoW(xml.GetAttrib("name"));
            sIcdVoltage = AtoW(xml.GetAttrib("voltage"));
        }
        xml.OutOfElem();
    }

    IedTemplateMap& mapIed = pCacheMgr->GetIedTemplateOfStandard();
    IedTemplate& ied = mapIed[sIcdName+sIcdVoltage];

    LDeviceTemplateMap& mapLD = ied.mapLDevice;
    LDeviceTemplateMap::iterator itLD = mapLD.begin();

    LDeviceCacheMap& mapLDInst = icd.mapLDevice;

    while (itLD != mapLD.end())
    {
        // ���ҵ���LD
        LDeviceCacheMap::iterator itLDInst = mapLDInst.find(itLD->first);
        if(itLDInst == mapLDInst.end())
        {
            pLnValidCheck->Report(
                pChecker->CalcRowNumberFromOffset(icd.offset),
                pChecker->GetRowContentByOffset(icd.offset),
                "ICD�ļ��в����ڱ�׼��Ҫ��ĵ��͡�LD:%s��",
                WtoA(itLD->first).c_str());
        }
        else
        {
            // ���ҵ���LD�еıر�LN
            LNTemplateMap& mapLN = itLD->second.mapLN;
            LNTemplateMap::iterator itLN = mapLN.begin();

            LNCacheMap& mapLNInst = itLDInst->second.mapLN;
            
            while (itLN != mapLN.end())
            {
                LNTemplate& ln = itLN->second;
                LNCacheMap::iterator itLNInst = mapLNInst.find(itLN->first);
                if(ln.sOption == L"M" && itLNInst == mapLNInst.end())
                {
                    pLnValidCheck->Report(
                        pChecker->CalcRowNumberFromOffset(itLDInst->second.offset),
                        pChecker->GetRowContentByOffset(itLDInst->second.offset),
                        "��LD:%s���²����ڱ�׼�еı�ѡ��LN:%s��",
                        WtoA(itLD->first).c_str(),
                        WtoA(itLN->second.sLnClass).c_str());
                }
                itLN++;
            }
        }
        itLD++;
    }
}