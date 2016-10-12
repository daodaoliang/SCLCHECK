#include "EnumtypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "145768"

CEnumtypeCheck::CEnumtypeCheck()
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

bool CEnumtypeCheck::CheckInternal()
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

    // ����׼EnumType�����ͺ�ֵ�Ƿ���ȷ
    EnumTypeMap::iterator itInstEnum = instEnumMap.begin();
    EnumTypeMap::iterator itTempEnum = tempEnumMap.begin();

    while (itInstEnum != instEnumMap.end()
        && (itTempEnum = tempEnumMap.find(itInstEnum->first)) != tempEnumMap.end())
    {
        vector<wstring>& instVal = itInstEnum->second.vecVal;
        vector<wstring>& tempVal = itTempEnum->second.vecVal;
        // �Ƚϸ�ö�����͵�ֵ
        if(instVal.size() != tempVal.size())
        {
            Report(
                pChecker->CalcRowNumberFromOffset(itInstEnum->second.offset),
                pChecker->GetRowContentByOffset(itInstEnum->second.offset),
                "ö�����͡�%s����ֵ��Ŀ���׼��һ��",
                WtoA(itInstEnum->first).c_str());

            itInstEnum++;
            itTempEnum++;

            continue;
        }

        for (unsigned int iVal = 0; iVal < instVal.size(); iVal++)
        {
            if (instVal[iVal] != tempVal[iVal])
            {
                //ֵ��һ��
                Report(
                    pChecker->CalcRowNumberFromOffset(itInstEnum->second.offset),
                    pChecker->GetRowContentByOffset(itInstEnum->second.offset),
                    "ö�����͡�%s����ֵ��%s�����׼��ֵ��%s����һ��",
                    WtoA(itInstEnum->first).c_str(),
                    WtoA(instVal[iVal]).c_str(),
                    WtoA(tempVal[iVal]).c_str());
            }
        }

        itInstEnum++;
        itTempEnum++;
    }

    return true;
}
