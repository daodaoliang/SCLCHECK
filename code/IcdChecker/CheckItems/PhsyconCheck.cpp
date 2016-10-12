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

    // 取本条目的配置信息并配置到本对象
    if (CConfigMgr::GetInstance()->GetItemConfig(KEY, cfg))
    {
        SetConfig(cfg);
    }

    // 如果启用了本条目，则向检测器注册
    if (IsEnabled())
    {
        CScdChecker::GetInstance()->RegisterCheckItem(this);

        // 在此处添加需要某某类型的缓存信息，还可以继续添加条目
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_Communication");
    }
}

bool CPhsyconCheck::CheckInternal()
{
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    // 输出管理器
    COutputMgr *pOutputMgr = COutputMgr::GetInstance();
    // 配置文件管理器
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
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();

    /*ST、SC、LC、FC、MTRJ、RJ45*/
    vector<wstring> vecPlug;
    vecPlug.push_back(L"ST");
    vecPlug.push_back(L"SC");
    vecPlug.push_back(L"LC");
    vecPlug.push_back(L"FC");
    vecPlug.push_back(L"MTRJ");
    vecPlug.push_back(L"RJ45");
    /*FOC、Radio、100BaseT*/
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
            "PhysConn配置的物理端口插头类型不符合规范中的命名要求：Plug = %s 应为以下值之一:ST、SC、LC、FC、MTRJ、RJ45",
            WtoA(sValue).c_str());

    }
    else if (sType == L"Type" && vecType.end() == find(vecType.begin(), vecType.end(), sValue))
    {
        pPhsyconCheck->Report(
            pChecker->CalcRowNumberFromOffset(offset),
            pChecker->GetRowContentByOffset(offset),
            "PhysConn配置的接口类型不符合规范中的命名要求：Type = %s 应为以下值之一:FOC、Radio、100BaseT",
            WtoA(sValue).c_str());
    }
}