#include "ConfDataSetCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "AF455E"

CConfDataSetCheck::CConfDataSetCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DataSet");
    }
}

bool CConfDataSetCheck::CheckInternal()
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

void CConfDataSetCheck::CheckDataset_InCacheMgr(ICDCache& icd)
{
    CConfDataSetCheck* pConfDataSetCheck = CConfDataSetCheck::GetInstance();

    if (!pConfDataSetCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();

    int maxDataset = icd.services.dataset_max;

    if (maxDataset == -1)
    {
        pConfDataSetCheck->Report(
            pChecker->CalcRowNumberFromOffset(icd.services.offset),
            pChecker->GetRowContentByOffset(icd.services.offset),
            "ConfDataset��max����δ����",
            maxDataset);
        return;
    }

    LDeviceCacheMap& devices = icd.mapLDevice;
    LDeviceCacheMap::iterator itLD = devices.begin();

    int nDataset = 0;
    while (itLD != devices.end())
    {
        LDeviceCache& device = itLD->second;
        nDataset += device.mapDataset.size();
        itLD++;
    }

    if (nDataset > maxDataset)
    {
        pConfDataSetCheck->Report(
            pChecker->CalcRowNumberFromOffset(icd.services.dataset_offset),
            pChecker->GetRowContentByOffset(icd.services.dataset_offset),
            "���ݼ�DataSet�ĸ�������ConfDataset��max����ֵ��%d��",
            maxDataset);
    }
}