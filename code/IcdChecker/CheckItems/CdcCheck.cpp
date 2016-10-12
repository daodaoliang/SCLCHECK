#include "CdcCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

#include "../../ScdCheckCommon/ValidateCache.h"

using namespace std;
using namespace com_ximpleware;

#define KEY "1F61CB"

CCdcCheck::CCdcCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfICD");
    }
}

bool CCdcCheck::CheckInternal()
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
    // �������е�DOType,�����CDC����
    DOTypeMap& instDOTypeMap = pCacheMgr->GetDOTypeCacheOfICD();

    DOTypeMap::iterator itInst = instDOTypeMap.begin();

    wchar_t* arrCdc[] = {L"DPC", L"INC", L"BSC", L"SPG",
                         L"ING", L"ASG", L"STG", L"DPL",
                         L"LPL", L"ACT", L"ACD", L"MV", L"CMV",
                         L"WYE", L"DEL", L"SPC", L"SAV", L"SPS", L"INS"};

    vector<wstring> vecCdc(arrCdc, arrCdc+sizeof(arrCdc)/sizeof(wchar_t*));

    while (itInst != instDOTypeMap.end())
    {
        wstring sCdc = itInst->second.cdc;

        if (vecCdc.end() == find(vecCdc.begin(), vecCdc.end(), sCdc))
        {
            // �Զ���CDC
            Report(
                pChecker->CalcRowNumberFromOffset(itInst->second.offset),
                pChecker->GetRowContentByOffset(itInst->second.offset),
                "��DOType:%s���д��������䡾CDC:%s��,���ڱ�׼��DPC,INC,BSC,SPG,ING,ASG,STG,DPL,LPL����",
                WtoA(itInst->second.id).c_str(),
                WtoA(sCdc).c_str());
        }
        itInst++;
    }

    return true;
}
