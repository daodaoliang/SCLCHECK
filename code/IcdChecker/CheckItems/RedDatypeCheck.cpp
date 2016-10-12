#include "RedDatypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "165A0B"

CRedDatypeCheck::CRedDatypeCheck()
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

bool CRedDatypeCheck::CheckInternal()
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

    // �õ� ��׼�ļ� �� ICD����ļ��е�DAType
    DATypeMap& instDAMap = pCacheMgr->GetDATypeCacheOfICD();
    DATypeMap& tempDAMap = pCacheMgr->GetDATypeCacheOfStandard();

    DATypeMap::iterator itDA = instDAMap.begin();
    /*
        ����Ƿ������չDA������չDAӦ���������淶
        �����쳧��ǰ׺_װ���ͺ�_�汾��_DA����(_������׺)��
    */
    while (itDA != instDAMap.end())
    {
        DATypeCache& da = tempDAMap[itDA->first];
        if (da.id.empty())
        {
            // ��չ DA
            wstring daName = da.id;
            vector<wstring>& vecSub = split_string(daName, L"_");
            if(vecSub.size() < 4)
            {
                Report(
                    pChecker->CalcRowNumberFromOffset(da.offset),
                    pChecker->GetRowContentByOffset(da.offset),
                    "%s",
                    "��չDAӦ���������淶�����쳧��ǰ׺_װ���ͺ�_�汾��_DA����(_������׺)��");
            }
        }
        itDA++;
    }

    return true;
}
