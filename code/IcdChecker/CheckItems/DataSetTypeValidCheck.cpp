#include "DataSetTypeValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "EA80EF"

CDataSetTypeValidCheck::CDataSetTypeValidCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_IED");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DataSet");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_FCDA");
    }
}

bool CDataSetTypeValidCheck::CheckInternal()
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

void CDataSetTypeValidCheck::CheckValid_InCacheMgr(wstring sCBType, FcdaCache& fcda)
{
    CDataSetTypeValidCheck* pDataSetTypeValidCheck = CDataSetTypeValidCheck::GetInstance();

    if (!pDataSetTypeValidCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    wstring sKey = fcda.sLdInst + L"/" + fcda.sPrefix + fcda.sLnClass + fcda.sLnInst + L"." + fcda.sDoName;
    if (!fcda.sDaName.empty())
    {
        sKey += L".";
        sKey += fcda.sDaName;
    }

    if(sCBType == L"GSEControl")
    {
        if(!fcda.bHasDoName || !fcda.bHasDaName)
        {
            pDataSetTypeValidCheck->Report(
                pChecker->CalcRowNumberFromOffset(fcda.offset),
                pChecker->GetRowContentByOffset(fcda.offset),
                "GOOSE���ݼ�δ���õ�DA��LDevice inst=%s, DataSet name=%s, FCDA=%s",
                WtoA(fcda.sLdInst).c_str(),
                WtoA(fcda.sDataset).c_str(),
                WtoA(sKey).c_str());
        }
    }
    else
    {
        if(!fcda.bHasDoName || fcda.bHasDaName)
        {
            pDataSetTypeValidCheck->Report(
                pChecker->CalcRowNumberFromOffset(fcda.offset),
                pChecker->GetRowContentByOffset(fcda.offset),
                "SV���ݼ�δ���õ�DO��LDevice inst=%s, DataSet name=%s, FCDA=%s",
                WtoA(fcda.sLdInst).c_str(),
                WtoA(fcda.sDataset).c_str(),
                WtoA(sKey).c_str());
        }
    }
}
