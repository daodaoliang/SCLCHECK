#include "PhsyconCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "ED195B"

CPhsyconCheck::CPhsyconCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_Communication");
    }
}

bool CPhsyconCheck::CheckInternal()
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

void CPhsyconCheck::CheckPortType_InCacheMgr(wstring sType, wstring sValue, int offset)
{
    CPhsyconCheck* pPhsyconCheck = CPhsyconCheck::GetInstance();

    if (!pPhsyconCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();

    /*ST��SC��LC��FC��MTRJ��RJ45*/
    vector<wstring> vecPlug;
    vecPlug.push_back(L"ST");
    vecPlug.push_back(L"SC");
    vecPlug.push_back(L"LC");
    vecPlug.push_back(L"FC");
    vecPlug.push_back(L"MTRJ");
    vecPlug.push_back(L"RJ45");
    /*FOC��Radio��100BaseT*/
    vector<wstring> vecType;
    vecType.push_back(L"FOC");
    vecType.push_back(L"Radio");
    vecType.push_back(L"100BaseT");

    vector<wstring>::iterator itFind;
    if (sType == L"Plug" && vecPlug.end() == find(vecPlug.begin(), vecPlug.end(), sValue))
    {
        pPhsyconCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "PhysConn���õ�����˿ڲ�ͷ���Ͳ����Ϲ淶�е�����Ҫ��Plug = %s ӦΪ����ֵ֮һ:ST��SC��LC��FC��MTRJ��RJ45",
            WtoA(sValue).c_str());

    }
    else if (sType == L"Type" && vecType.end() == find(vecType.begin(), vecType.end(), sValue))
    {
        pPhsyconCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "PhysConn���õĽӿ����Ͳ����Ϲ淶�е�����Ҫ��Type = %s ӦΪ����ֵ֮һ:FOC��Radio��100BaseT",
            WtoA(sValue).c_str());
    }
}