#include "RedDotypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "A63491"

CRedDotypeCheck::CRedDotypeCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfICD");
    }
}
/*
����Ƿ������չ��DO����չʱӦ���������淶
�����쳧��ǰ׺_װ���ͺ�_�汾��_DO����(_������׺)��
*/
void CRedDotypeCheck::CheckDOType(DOTypeCache& instDOType)
{
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DOTypeMap& tempDOTypeMap = pCacheMgr->GetDOTypeCacheOfStandard();
    
    DOTypeMap::iterator itTempDOType = tempDOTypeMap.find(instDOType.id);
    
    if (itTempDOType != tempDOTypeMap.end())
    {
        return;
    }
    // ��չDO
    vector<wstring>& vecName = split_string(instDOType.id,L"_");
    if(vecName.size() < 4)
    {
        Report(
            pChecker->CalcRowNumberFromOffset(instDOType.offset),
            pChecker->GetRowContentByOffset(instDOType.offset),
            "%s",
            "��չ��DOӦ���������淶�����쳧��ǰ׺_װ���ͺ�_�汾��_DO����(_������׺))"
            );
    }
}

bool CRedDotypeCheck::CheckInternal()
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
    // �õ�ICD����ļ��е�DOType
    DOTypeMap& instDOMap = pCacheMgr->GetDOTypeCacheOfICD();
    DOTypeMap::iterator itDO = instDOMap.begin();
    while(itDO != instDOMap.end())
    {
        CheckDOType(itDO->second);

        itDO++;
    }

    return true;
}
