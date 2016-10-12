#include "LdValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "6A2E19"

CLdValidCheck::CLdValidCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetLDeviceTypeOfStandard");

        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_IED");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
    }
}

bool CLdValidCheck::CheckInternal()
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

void CLdValidCheck::CheckName_InCacheMgr(wstring sName, int offset)
{
    CLdValidCheck* pLdValidCheck = CLdValidCheck::GetInstance();

    if (!pLdValidCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    LDeviceTypeMap& mapLDevice = pCacheMgr->GetLDeviceTypeOfStandard();

    LDeviceTypeMap::iterator itLDevice = mapLDevice.begin();

    while (itLDevice != mapLDevice.end())
    {
        wstring sNameTemp = itLDevice->second.sName;
        int iPos = sName.find(sNameTemp);
        if (iPos == 0)
        {
            return;
        }
        itLDevice++;
    }

    pLdValidCheck->Report(
        pChecker->CalcRowNumberFromOffset(offset),
        pChecker->GetRowContentByOffset(offset),
        "%s",
        "LDevice������Ӧ����Q/GDW-1396��׼�ж��������ԭ��");

}