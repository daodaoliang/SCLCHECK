#include "XmlCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "CE18E6"

CXmlCheck::CXmlCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark(KEY);
    }
}

bool CXmlCheck::CheckInternal()
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
    try
    {
        VTDGen vg;

        if (vg.parseFile(false, pChecker->GetFilePathToCheck().c_str()))
        {
            VTDNav *pVn = vg.getNav();

            if (pVn != NULL)
            {
                pChecker->SetExistingVtdNav(pVn);
                pChecker->StoreSizeOfFileToCheck(GetSmallFileSize(pChecker->GetFilePathToCheck()));
                return true;
            }
        }
        else
        {
            delete[] vg.getXML();
        }
    }
    catch (std::exception &e)
    {
        COutputMgr::GetInstance()->Error("CXmlCheck::CheckInternal异常%s\r\n", e.what());
    }

    Report(0, "", "xml格式错误");

    return true;
}
