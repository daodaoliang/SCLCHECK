#include "DoiValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "28F210"

CDoiValidCheck::CDoiValidCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DOI");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_SDI");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DAI");
    }
}

bool CDoiValidCheck::CheckInternal()
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

void CDoiValidCheck::CheckDescAnddU_InCacheMgr(LNCache& ln, DOICache& doi, wstring sdU, int offset)
{
    CDoiValidCheck* pDoiValidCheck = CDoiValidCheck::GetInstance();

    if (!pDoiValidCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    if (doi.sdesc.empty() || sdU.empty() || doi.sdesc != sdU)
    {
        pDoiValidCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "ʵ����DOIԪ�صġ�desc������ֵ�롰dU������ֵ��һ��:LDevice inst=%s LN=%s DOI name=%s desc=\"%s\" dU=\"%s\" ",
            WtoA(ln.sLdInst).c_str(),
            WtoA(ln.sLnClass+ln.sInst).c_str(),
            WtoA(doi.sname).c_str(),
            WtoA(doi.sdesc).c_str(),
            WtoA(sdU).c_str());
    }
}