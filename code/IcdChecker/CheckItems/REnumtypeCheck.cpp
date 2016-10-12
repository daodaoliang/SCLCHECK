#include "REnumtypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "56AB89"

CREnumtypeCheck::CREnumtypeCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetEnumTypeCacheOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetEnumTypeCacheOfICD");
    }
}

bool CREnumtypeCheck::CheckInternal()
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
    // �õ���׼��ICD�е�EnumType
    EnumTypeMap& tempEnumMap = pCacheMgr->GetEnumTypeCacheOfStandard();
    EnumTypeMap& instEnumMap = pCacheMgr->GetEnumTypeCacheOfICD();

    // ����Ƿ������չ��EnumType
    EnumTypeMap::iterator itInstEnum = instEnumMap.begin();
    EnumTypeMap::iterator itTempEnum = tempEnumMap.begin();

    while (itInstEnum != instEnumMap.end())
    {
        itTempEnum = tempEnumMap.find(itInstEnum->first);
        if (itTempEnum == tempEnumMap.end())
        {
            // ��չEnumType
            Report(
                pChecker->CalcRowNumberFromOffset(itInstEnum->second.offset),
                pChecker->GetRowContentByOffset(itInstEnum->second.offset),
                "������չ��EnumType��%s��",
                WtoA(itInstEnum->first).c_str());
        }
        itInstEnum++;
    }

    return true;
}
