#include "FcdaLdValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "58E4DA"

CFcdaLdValidCheck::CFcdaLdValidCheck()
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

bool CFcdaLdValidCheck::CheckInternal()
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

void CFcdaLdValidCheck::CheckCrossLDevice(FcdaCache& fcda, wstring sLd)
{
    CFcdaLdValidCheck* pFcdaLdValidCheck = CFcdaLdValidCheck::GetInstance();

    if (!pFcdaLdValidCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    if (fcda.sLdInst != sLd)
    {
        wstring sKey = fcda.sLdInst + L"/" + fcda.sPrefix + fcda.sLnClass + fcda.sLnInst + L"." + fcda.sDoName;
        if (!fcda.sDaName.empty())
        {
            sKey += L".";
            sKey += fcda.sDaName;
        }

        pFcdaLdValidCheck->Report(
            pChecker->CalcRowNumberFromOffset(fcda.offset),
            pChecker->GetRowContentByOffset(fcda.offset),
            "���ݼ��а�����FCDA����LDevice�У�LDevice inst = %s  FCDA=%s",
            WtoA(fcda.sLdInst).c_str(),
            WtoA(sKey).c_str());
    }
}