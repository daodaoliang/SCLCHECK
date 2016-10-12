#include "LDdoiCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "E9DFBC"

CLDdoiCheck::CLDdoiCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetLNodeTypeCacheOfICD");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfICD");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDATypeCacheOfICD");

        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_IED");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DOI");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_SDI");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DAI");
    }
}

bool CLDdoiCheck::CheckInternal()
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

bool CLDdoiCheck::CheckLnTypeValid_InCacheMgr(LNCache& ln, DOMap& mapDo)
{
    CLDdoiCheck* pLDdoiCheck = CLDdoiCheck::GetInstance();

    if (!pLDdoiCheck->IsEnabled())
    {
        return false;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    LNodeTypeMap& mapLNodeType = pCacheMgr->GetLNodeTypeCacheOfICD();
    LNodeTypeMap::iterator itLNodeType = mapLNodeType.find(ln.sLnType);

    if (itLNodeType == mapLNodeType.end())
    {
        pLDdoiCheck->Report(
            pChecker->CalcRowNumberFromOffset(ln.offset),
            pChecker->GetRowContentByOffset(ln.offset),
            "��lnType:%s������������ģ���в�����",
            WtoA(ln.sLnType).c_str());
        return false;
    }
    else
    {
        mapDo = itLNodeType->second.mapDO;
    }
    return true;
}

bool CLDdoiCheck::CheckDOTypeValid_InCacheMgr(DOICache& doi, DOMap& mapDo, DOTypeCache& doType)
{
    CLDdoiCheck* pLDdoiCheck = CLDdoiCheck::GetInstance();

    if (!pLDdoiCheck->IsEnabled())
    {
        return false;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DOTypeMap& mapDoType = pCacheMgr->GetDOTypeCacheOfICD();

    wstring sDoType = mapDo[doi.sname].type;
    DOTypeMap::iterator itDoType = mapDoType.find(sDoType);

    if (sDoType.empty())
    {
        pLDdoiCheck->Report(
            pChecker->CalcRowNumberFromOffset(doi.offset),
            pChecker->GetRowContentByOffset(doi.offset),
            "��doName:%s������������ģ���в�����",
            WtoA(doi.sname).c_str());
        return false;
    }
    else if (itDoType == mapDoType.end())
    {
        pLDdoiCheck->Report(
            pChecker->CalcRowNumberFromOffset(doi.offset),
            pChecker->GetRowContentByOffset(doi.offset),
            "��doType:%s������������ģ���в�����",
            WtoA(sDoType).c_str());
        return false;
    }
    else
    {
        doType = itDoType->second;
    }
    return true;
}

void CLDdoiCheck::CheckDATypeValid_InCacheMgr(int offset, wstring sDaName, SdiCache& sdi)
{
    CLDdoiCheck* pLDdoiCheck = CLDdoiCheck::GetInstance();

    if (!pLDdoiCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DATypeMap& mapDaType = pCacheMgr->GetDATypeCacheOfICD();

    DAMap& mapDa = sdi.mapDaType;
    BDAMap& mapBda = sdi.mapBDA;

    if (sdi.pos == SdiCache::IN_DO && mapDa.end() == mapDa.find(sDaName))
    {
        pLDdoiCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "��daName:%s������������ģ���в�����",
            WtoA(sDaName).c_str());
    }
    else if (sdi.pos == SdiCache::IN_DA && mapBda.end() == mapBda.find(sDaName))
    {
        pLDdoiCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "��daName:%s������������ģ���в�����",
            WtoA(sDaName).c_str());
    }
}


bool CLDdoiCheck::CheckSDITypeValid_InCacheMgr(int offset, wstring sSdiName, SdiCache& sdi)
{
    CLDdoiCheck* pLDdoiCheck = CLDdoiCheck::GetInstance();

    if (!pLDdoiCheck->IsEnabled())
    {
        return false;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DOTypeMap& mapDoType = pCacheMgr->GetDOTypeCacheOfICD();
    DATypeMap& mapDaType = pCacheMgr->GetDATypeCacheOfICD();
    /*
            �ж� sdi������ sdo������da ����
    */
    DAMap& mapDa = sdi.mapDaType;
    DOMap& mapDo = sdi.mapSDO;
    BDAMap& mapBda = sdi.mapBDA;

    if (sdi.pos == SdiCache::IN_DO)
    {
        if (mapDa.find(sSdiName) != mapDa.end())
        {
            sdi.pos = SdiCache::IN_DA;
            wstring sDaType = mapDa[sSdiName].type;
            sdi.mapBDA = mapDaType[sDaType].mapBDA;
        }
        else if (mapDo.find(sSdiName) != mapDo.end())
        {
            wstring sDoType = mapDo[sSdiName].type;
            sdi.mapSDO = mapDoType[sDoType].mapSDO;
            sdi.mapDaType = mapDoType[sDoType].mapDaType;
        }
        else
        {
            pLDdoiCheck->Report(
                pChecker->CalcRowNumberFromOffset(offset),
                pChecker->GetRowContentByOffset(offset),
                "��SDI Name:%s������������ģ���в�����",
                WtoA(sSdiName).c_str());
            return false;
        }
    }
    else if (sdi.pos == SdiCache::IN_DA)
    {
        if (mapBda.find(sSdiName) != mapBda.end())
        {
            wstring sBadType = mapBda[sSdiName].type;
            sdi.mapBDA = mapDaType[sBadType].mapBDA;
        }
        else
        {
            pLDdoiCheck->Report(
                pChecker->CalcRowNumberFromOffset(offset),
                pChecker->GetRowContentByOffset(offset),
                "��bdaName:%s������������ģ���в�����",
                WtoA(sSdiName).c_str());
            return false;
        }
    }
    return true;
}