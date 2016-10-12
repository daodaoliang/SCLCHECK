#include "LDescBlankCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "71A77B"

CLDescBlankCheck::CLDescBlankCheck()
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
        // CCacheMgr::GetInstance()->AddNeededCacheMark("LogicNodeInstance");
    }
}

bool CLDescBlankCheck::CheckInternal()
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
    CScopedVtdNav m_vn = pChecker->GetExistingVtdNav();
    m_vn->toElement(ROOT);
    if (m_vn->toElement(FIRST_CHILD, L"IED"))
    {
        do 
        {
            wstring iedName = VtdGetAttributeValue(m_vn, L"name");
            if (m_vn->toElement(FIRST_CHILD, L"AccessPoint"))
            {
                do 
                {
                    if (m_vn->toElement(FIRST_CHILD, L"Server"))
                    {
                        if (m_vn->toElement(FIRST_CHILD, L"LDevice"))
                        {
                            do 
                            {
                                wstring ldInst = VtdGetAttributeValue(m_vn, L"inst");
                                wstring lddesc = VtdGetAttributeValue(m_vn, L"desc");
                                if (lddesc.size() == 0)
                                {
                                    int offset = (m_vn->getElementFragment() & 0xFFFFFFFF);
                                    int row = pChecker->CalcRowNumberFromOffset(offset);
                                    string xml = pChecker->GetRowContentByOffset(offset);
                                    string msg = "逻辑设备LD的“desc”属性值为空:IED name=%s LDevice inst=%s";
                                    Report(row, xml, msg.c_str(),
                                        WtoA(iedName).c_str(), WtoA(ldInst).c_str());
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"LN0"))
                                {
                                    wstring ln0desc = VtdGetAttributeValue(m_vn, L"desc");
                                    if (ln0desc.size() == 0)
                                    {
                                        int offset = (m_vn->getElementFragment() & 0xFFFFFFFF);
                                        int row = pChecker->CalcRowNumberFromOffset(offset);
                                        string xml = pChecker->GetRowContentByOffset(offset);
                                        string msg = "逻辑节点LN0的“desc”属性值为空:IED name=%s LDevice inst=%s";
                                        Report(row, xml, msg.c_str(),
                                            WtoA(iedName).c_str(), WtoA(ldInst).c_str());
                                    }
                                    m_vn->toElement(PARENT);
                                }
                                if (m_vn->toElement(FIRST_CHILD, L"LN"))
                                {
                                    do 
                                    {
                                        wstring lnClass = VtdGetAttributeValue(m_vn, L"lnClass");
                                        wstring prefix = VtdGetAttributeValue(m_vn, L"prefix");
                                        wstring inst = VtdGetAttributeValue(m_vn, L"inst");
                                        wstring ln = prefix + lnClass + inst;
                                        wstring lndesc = VtdGetAttributeValue(m_vn, L"desc");
                                        if (lndesc.size() == 0)
                                        {
                                            int offset = (m_vn->getElementFragment() & 0xFFFFFFFF);
                                            int row = pChecker->CalcRowNumberFromOffset(offset);
                                            string xml = pChecker->GetRowContentByOffset(offset);
                                            string msg = "逻辑节点的“desc”属性值为空:IED name=%s LDevice inst=%s LN=%s";
                                            Report(row, xml, msg.c_str(), 
                                                WtoA(iedName).c_str(), WtoA(ldInst).c_str(), WtoA(ln).c_str());
                                        }
                                    } while (m_vn->toElement(NEXT_SIBLING, L"LN"));
                                    m_vn->toElement(PARENT);
                                }
                            } while (m_vn->toElement(NEXT_SIBLING, L"LDevice"));
                            m_vn->toElement(PARENT);
                        }
                        m_vn->toElement(PARENT);
                    }
                } while (m_vn->toElement(NEXT_SIBLING, L"AccessPoint"));
                m_vn->toElement(PARENT);
            }
        } while (m_vn->toElement(NEXT_SIBLING, L"IED"));
        m_vn->toElement(PARENT);
    }

    return true;
}

