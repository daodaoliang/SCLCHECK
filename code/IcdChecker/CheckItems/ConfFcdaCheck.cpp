#include "ConfFcdaCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "B3824A"

CConfFcdaCheck::CConfFcdaCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_FCDA");
    }
}

bool CConfFcdaCheck::CheckInternal()
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

void CConfFcdaCheck::CheckFcdaCount_InCacheMgr(DatasetCache& dataset, int maxFcda)
{
    CConfFcdaCheck* pConfFcdaCheck = CConfFcdaCheck::GetInstance();

    if (!pConfFcdaCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    int nFcdaCount = dataset.vecFcda.size();
    if (maxFcda == -1)
    {
        pConfFcdaCheck->Report(
            pChecker->CalcRowNumberFromOffset(dataset.offset),
            pChecker->GetRowContentByOffset(dataset.offset),
            "ConfDataSet��maxAttributes����δ����"
            );
        return;
    }
    if (nFcdaCount > maxFcda)
    {
        pConfFcdaCheck->Report(
            pChecker->CalcRowNumberFromOffset(dataset.offset),
            pChecker->GetRowContentByOffset(dataset.offset),
            "���ݼ����õ�FCDAʵ�ʸ�����%d������ConfDataset��maxAttributes����ֵ��AccessPoint=%s LDevice=%s DataSet=%s ConfDataset(maxAttributes)=%d ",
            nFcdaCount,
            WtoA(dataset.sAccessPoint).c_str(),
            WtoA(dataset.sLDevice).c_str(),
            WtoA(dataset.sName).c_str(),
            maxFcda);
    }
    else if (nFcdaCount == 0)
    {
        pConfFcdaCheck->Report(
            pChecker->CalcRowNumberFromOffset(dataset.offset),
            pChecker->GetRowContentByOffset(dataset.offset),
            "���ݼ����õ�FCDA������%d������Ϊ 0 ��AccessPoint=%s LDevice=%s DataSet=%s ",
            nFcdaCount,
            WtoA(dataset.sAccessPoint).c_str(),
            WtoA(dataset.sLDevice).c_str(),
            WtoA(dataset.sName).c_str());
    }
}