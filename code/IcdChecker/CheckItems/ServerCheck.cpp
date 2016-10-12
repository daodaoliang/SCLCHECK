#include "ServerCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "9A2FE1"

CServerCheck::CServerCheck()
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
    }
}

bool CServerCheck::CheckInternal()
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

/*
        ÿ��LDevice��������Ӧ����LLN0��LPHD�����߼��ڵ�
*/
void CServerCheck::CheckLLN0LPHD_InCacheMgr(LDeviceCache& lDevice)
{
    CServerCheck* pServerCheck = CServerCheck::GetInstance();

    if (!pServerCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();

    if (!lDevice.bLLN0 || !lDevice.bLPHD)
    {
        pServerCheck->Report(
            pChecker->CalcRowNumberFromOffset(lDevice.offset),
            pChecker->GetRowContentByOffset(lDevice.offset),
            "LDevice��������Ӧ����LLN0��LPHD�����߼��ڵ�"
            );
    }
}
/*
        Server����������Ӧ����һ��LDevice����
*/
void CServerCheck::CheckhasLDevice_InCacheMgr(bool bHasLDevice, int offset)
{
    CServerCheck* pServerCheck = CServerCheck::GetInstance();

    if (!pServerCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();

    if (bHasLDevice == false)
    {
        pServerCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "Server����������Ӧ����һ��LDevice����"
            );
    }
}