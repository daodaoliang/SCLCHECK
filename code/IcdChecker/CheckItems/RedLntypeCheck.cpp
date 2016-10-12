#include "RedLntypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

#include <tchar.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "2C98FA"

CRedLntypeCheck::CRedLntypeCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetLNodeTypeCacheOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetLNodeTypeCacheOfICD");
    }
}

/*
����Ƿ������չ��LNodeType��
1����չ��LNodeTypeӦ���������淶�����쳧��ǰ׺_װ���ͺ�_�汾��_LN����(_������׺)��
2����չLNodeType�������ռ�Ҫ����
*/
void CRedLntypeCheck::CheckExtendLNodeType(LNodeTypeCache& instLNodeType)
{
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();

    wstring sLNodeTypeName = instLNodeType.id;

    vector<wstring> vecNames = split_string(sLNodeTypeName, L"_");

    if (vecNames.size() < 4)
    {
        // 1����չ��LNodeTypeӦ���������淶�����쳧��ǰ׺_װ���ͺ�_�汾��_LN����(_������׺)��
        Report(
            pChecker->CalcRowNumberFromOffset(instLNodeType.offset),
            pChecker->GetRowContentByOffset(instLNodeType.offset),
            "%s",
            "��չ��LNodeTypeӦ���������淶�����쳧��ǰ׺_װ���ͺ�_�汾��_LN����(_������׺)��"
            );
    }
    // 2����չLNodeType�������ռ�Ҫ����

}

bool CRedLntypeCheck::CheckInternal()
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
    // �õ� ��׼��LNodeType �� �����ICD�ļ��е�LNodeType
    LNodeTypeMap& instLNodeType = pCacheMgr->GetLNodeTypeCacheOfICD();
    LNodeTypeMap& tempLNodeType = pCacheMgr->GetLNodeTypeCacheOfStandard();

    LNodeTypeMap::iterator itInst = instLNodeType.begin();
    LNodeTypeMap::iterator itTemp;

    while (itInst != instLNodeType.end())
    {
        wstring sId = itInst->second.lnClass;

        itTemp = tempLNodeType.find(sId);
        // ��չ LNodeType
        if (itTemp == tempLNodeType.end()) 
        {
            CheckExtendLNodeType(itInst->second);
        }
        itInst++;
    }

    return true;
}
