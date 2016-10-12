#include "MDatypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "B73E6D"

CMDatypeCheck::CMDatypeCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDATypeCacheOfICD");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDATypeCacheOfStandard");
    }
}

bool CMDatypeCheck::CheckInternal()
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
    /*
        ��������ģ�����׼DAType��ǿ��BDAԪ���Ƿ����
    */
    // �õ� ��׼�ļ� �� ICD����ļ��е�DAType
    DATypeMap& instDAMap = pCacheMgr->GetDATypeCacheOfICD();
    DATypeMap& tempDAMap = pCacheMgr->GetDATypeCacheOfStandard();

    DATypeMap::iterator itDA = instDAMap.begin();
    // ����DA
    while (itDA != instDAMap.end())
    {        
        DATypeCache& instDA = itDA->second;
        DATypeCache& tempDA = tempDAMap[itDA->first];
        if (tempDA.id.empty())
        {
            itDA++;
            continue;
        }
        // ����BDA
        BDAMap& instBDA = instDA.mapBDA;
        BDAMap& tempBDA = tempDA.mapBDA;
        BDAMap::iterator itBDA = tempBDA.begin();
        while (itBDA != tempBDA.end())
        {
            if(itBDA->second.eOption == L"M"
                && instBDA.find(itBDA->first) == instBDA.end())
            {
                // ǿ��BDA������
                Report(
                    pChecker->CalcRowNumberFromOffset(instDA.offset),
                    pChecker->GetRowContentByOffset(instDA.offset),
                    "��׼��DA:%s����ǿ�ơ�BDA:%s��������",
                    WtoA(instDA.id).c_str(),
                    WtoA(itBDA->first).c_str());
            }
            itBDA++;
        }
        itDA++;
    }
    return true;
}
