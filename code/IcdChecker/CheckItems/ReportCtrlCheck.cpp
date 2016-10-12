#include "ReportCtrlCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "8E9CE7"

CReportCtrlCheck::CReportCtrlCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetReportControlTypeOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_IED");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
    }
}

bool CReportCtrlCheck::CheckInternal()
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

void CReportCtrlCheck::CheckName_InCacheMgr(wstring sName, int offset)
{
    CReportCtrlCheck* pReportCtrlCheck = CReportCtrlCheck::GetInstance();

    if (!pReportCtrlCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    ReportControlTypeMap& mapReport = pCacheMgr->GetReportControlTypeOfStandard();

    ReportControlTypeMap::iterator itReport = mapReport.begin();

    while (itReport != mapReport.end())
    {
        wstring sNameTemp = itReport->second.sName;
        int iPos = sName.find(sNameTemp);
        if (iPos == 0)
        {
            return;
        }
        itReport++;
    }

    pReportCtrlCheck->Report(
        pChecker->CalcRowNumberFromOffset(offset),
        pChecker->GetRowContentByOffset(offset),
        "������ƿ������������Q/GDW-1396��׼(ǰ׺+����): ReportControl name=%s",
        WtoA(sName).c_str()
        );
}