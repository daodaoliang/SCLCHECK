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
    }
}

bool CCrcBlankCheck::CheckInternal()
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
            string msg = "ȫվ�����CRCУ���벻����";
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
                                string msg = "IEDװ�õ�CRCУ�����ظ�:IED name=%s";
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
                    string msg = "IEDװ�õ�CRCУ���벻����:IED name=%s";
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
