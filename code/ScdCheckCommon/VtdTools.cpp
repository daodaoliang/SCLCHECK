#include "VtdTools.h"
#include "charconv.h"

CScopedVtdNav::CScopedVtdNav(VTDNav *vn)
    : m_vn(NULL)
{
    if (vn == NULL)
    {
        throw std::runtime_error("CScopedVtdNav::CScopedVtdNav()'s vn is nullptr");
    }

    m_vn = vn->duplicateNav();
}

CScopedVtdNav::~CScopedVtdNav()
{
    if (m_vn != NULL)
    {
        delete m_vn;
    }
}

CScopedVtdNav::operator VTDNav *()
{
    return m_vn;
}

CScopedVtdNav::operator const VTDNav *() const
{
    return m_vn;
}

VTDNav *CScopedVtdNav::operator->()
{
    return m_vn;
}

const VTDNav *CScopedVtdNav::operator->() const
{
    return m_vn;
}

std::wstring VtdGetString(VTDNav *vn, int i)
{
    wstring ret;

    if (i != -1)
    {
        const UCSChar *lpValue = vn->toString(i);

        if (lpValue != NULL)
        {
            ret = lpValue;
            delete[] lpValue;
        }
    }

    return ret;
}

std::wstring VtdGetAttributeValue(VTDNav *vn, const wchar_t *lpAttributeName)
{
    return VtdGetString(vn, vn->getAttrVal(lpAttributeName));
}

std::wstring VtdGetTextValue(VTDNav *vn)
{
    return VtdGetString(vn, vn->getText());
}

#define EMPTY_STRING ""

string GetXmlAttrVal(VTDNav *vn, const UCSChar *attr_name)
{
    string attrVal;
    int valIndex = vn->getAttrVal(attr_name);

    if (valIndex!=-1)
    {
        attrVal = WtoA(vn->toString(valIndex));
    } 
    else
    {
        attrVal = EMPTY_STRING;
    }
    return attrVal;
}

int VtdGetAttributeCount(VTDNav *vn)
{
    return vn->getAttrCount();
}

std::wstring VtdGetElementName(VTDNav *vn)
{
    wstring eleName = L"";

    int index = vn->getCurrentIndex();
    if (index != -1)
    {
        const UCSChar *lpValue = vn->toString(index);

        if (lpValue != NULL)
        {
            eleName = lpValue;
            delete[] lpValue;
        }
    }
    return eleName;
}
