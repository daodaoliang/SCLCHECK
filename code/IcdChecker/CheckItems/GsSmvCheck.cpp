#include "GsSmvCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "467747"

CGsSmvCheck::CGsSmvCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
    }
}

bool CGsSmvCheck::CheckInternal()
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
// IED��Ӧ����GSEControl��SampledValueControl
void CGsSmvCheck::CheckGseSmv_InCacheMgr(ICDCache& icd)
{
    CGsSmvCheck* pGsSmvCheck = CGsSmvCheck::GetInstance();

    if (!pGsSmvCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();

    LDeviceCacheMap& mapLd = icd.mapLDevice;
    LDeviceCacheMap::iterator itLd = mapLd.begin();

    while (itLd != mapLd.end())
    {
        LDeviceCache& device = itLd->second;

        if(device.bHasGoose || device.bHasSmv)
        {
            return;
        }
        itLd++;
    }

    pGsSmvCheck->Report(
        pChecker->CalcRowNumberFromOffset(0),
        pChecker->GetRowContentByOffset(0),
        "IED�в�����GSEControl��SampledValueCotrol,Ӧ���ٰ�������һ��");
}