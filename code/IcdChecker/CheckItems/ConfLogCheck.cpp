#include "ConfLogCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "B21624"

CConfLogCheck::CConfLogCheck()
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

bool CConfLogCheck::CheckInternal()
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


void CConfLogCheck::CheckLogCB_InCacheMgr(ICDCache& icd)
{
    CConfLogCheck* pConfLogCheck = CConfLogCheck::GetInstance();

    if (!pConfLogCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();

    int maxLog = icd.services.log_max;

    if (maxLog == -1)
    {
        pConfLogCheck->Report(
            pChecker->CalcRowNumberFromOffset(icd.services.offset),
            pChecker->GetRowContentByOffset(icd.services.offset),
            "ConflogControl��max����δ����");
        return;
    }

    LDeviceCacheMap& devices = icd.mapLDevice;
    LDeviceCacheMap::iterator itLD = devices.begin();

    int nLog = 0;
    while (itLD != devices.end())
    {
        LDeviceCache& device = itLD->second;
        nLog += device.nLogCB;
        itLD++;
    }

    if (nLog > maxLog)
    {
        pConfLogCheck->Report(
            pChecker->CalcRowNumberFromOffset(icd.services.log_offset),
            pChecker->GetRowContentByOffset(icd.services.log_offset),
            "��־���ƿ�LogControl�ĸ�������ConflogControl��max����ֵ��LogControl����= %d  ConflogControl(max)= %d ",
            nLog, maxLog);
    }
}