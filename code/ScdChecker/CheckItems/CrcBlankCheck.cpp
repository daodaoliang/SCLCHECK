#include "CrcBlankCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "DECEFE"

CCrcBlankCheck::CCrcBlankCheck()
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
    }
}

bool CCrcBlankCheck::CheckInternal()
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
    VTDNav *m_vn = pChecker->GetExistingVtdNav();
    bool isSCDExist = false;
    try
    {
        m_vn->toElement(ROOT);
        int offset = m_vn->getElementFragment() & 0xFFFFFFFF;
        if (m_vn->toElement(FIRST_CHILD, L"Private"))
        {
            offset = m_vn->getElementFragment() & 0xFFFFFFFF;
            wstring type = VtdGetAttributeValue(m_vn, L"type");
            if (type.compare(L"Substation virtual terminal conection CRC") == 0)
            {
                wstring txt = VtdGetTextValue(m_vn);
                if (txt.size() > 0)
                {
                    isSCDExist = true;
                }
            }
            m_vn->toElement(PARENT);
        }
        if (!isSCDExist)
        {
            int row = pChecker->CalcRowNumberFromOffset(offset);
            string xml = pChecker->GetRowContentByOffset(offset);
            string msg = "全站虚端子CRC校验码不存在";
            Report(row, xml, msg.c_str());
        }

        if (m_vn->toElement(FIRST_CHILD, L"IED"))
        {
            do 
            {
                bool isIEDExist = false;
                int offset = m_vn->getElementFragment() & 0xFFFFFFFF;
                wstring iedName = VtdGetAttributeValue(m_vn, L"name");
                if (m_vn->toElement(FIRST_CHILD, L"Private"))
                {
                    do 
                    {
                        wstring type = VtdGetAttributeValue(m_vn, L"type");
                        if (type.compare(L"IED virtual terminal conection CRC") == 0)
                        {
                            if (isIEDExist)
                            {
                                int row = pChecker->CalcRowNumberFromOffset(offset);
                                string xml = pChecker->GetRowContentByOffset(offset);
                                string msg = "IED装置的CRC校验码重复:IED name=%s";
                                Report(row, xml, msg.c_str(), WtoA(iedName).c_str());
                            }
                            wstring txt = VtdGetTextValue(m_vn);
                            if (txt.size() > 0)
                            {
                                isIEDExist = true;
                            }
                        }
                    } while (m_vn->toElement(NEXT_SIBLING, L"Private"));
                    m_vn->toElement(PARENT);
                }
                if (!isIEDExist)
                {
                    int row = pChecker->CalcRowNumberFromOffset(offset);
                    string xml = pChecker->GetRowContentByOffset(offset);
                    string msg = "IED装置的CRC校验码不存在:IED name=%s";
                    Report(row, xml, msg.c_str(), WtoA(iedName).c_str());
                }
            } while (m_vn->toElement(NEXT_SIBLING, L"IED"));
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}
