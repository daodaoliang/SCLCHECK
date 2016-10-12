#include "ConfReportCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "0CB373"

CConfReportCheck::CConfReportCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_Services");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
    }
}

bool CConfReportCheck::CheckInternal()
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

void CConfReportCheck::CheckReportCB_InCacheMgr(ICDCache& icd)
{
    CConfReportCheck* pConfReportCheck = CConfReportCheck::GetInstance();

    if (!pConfReportCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();

    int maxReport = icd.services.report_max;
    if (maxReport == -1)
    {
        pConfReportCheck->Report(
            pChecker->CalcRowNumberFromOffset(icd.services.offset),
            pChecker->GetRowContentByOffset(icd.services.offset),
            "ConfReportControl��max����δ����");
        return;
    }

    LDeviceCacheMap& devices = icd.mapLDevice;
    LDeviceCacheMap::iterator itLD = devices.begin();
    int nReport = 0;
    while (itLD != devices.end())
    {
        LDeviceCache& device = itLD->second;
        nReport += device.nReportCB;
        itLD++;
    }

    if (nReport > maxReport)
    {
        pConfReportCheck->Report(
            pChecker->CalcRowNumberFromOffset(icd.services.report_offset),
            pChecker->GetRowContentByOffset(icd.services.report_offset),
            "������ƿ�ReportControl�ĸ�������ConfReportControl��max����ֵ��ReportControl����= %d  ConfReportControl(max)= %d ",
            nReport,
            maxReport);
    }
}