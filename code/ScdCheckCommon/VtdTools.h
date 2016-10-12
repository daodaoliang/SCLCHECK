// �����⿪��

#ifndef _VTDTOOLS_H
#define _VTDTOOLS_H

#include "../VtdXml/everything.h"
#include <string>
#include <sstream>

// todo���˴���ͷ�ļ��������������ռ䣬�Ƿ��б�Ҫ�Ƴ���
using namespace com_ximpleware;

// ½�ڣ��������Զ��������
class CScopedVtdNav
{
public:
    CScopedVtdNav(VTDNav *vn);
    virtual ~CScopedVtdNav();
    
public:
    operator VTDNav *();
    operator const VTDNav *() const;
    VTDNav *operator->();
    const VTDNav *operator->() const;

protected:
    VTDNav *m_vn;
};

// ½�ڣ���ȡ��������ָ��id���ַ���
std::wstring VtdGetString(VTDNav *vn, int i);

// ½�ڣ���ȡ�������е�ǰλ��ָ�����Ƶ�����ֵ
std::wstring VtdGetAttributeValue(VTDNav *vn, const wchar_t *lpAttributeName);

// zxc ��ȡ��������
int VtdGetAttributeCount(VTDNav *vn);

// zxc ��ȡ��ǰElement������
std::wstring VtdGetElementName(VTDNav *vn);

// ½�ڣ���ȡ�������е�ǰλ�ö�Ӧ���ַ���
std::wstring VtdGetTextValue(VTDNav *vn);

// ��ǿ
std::string GetXmlAttrVal(VTDNav *vn, const UCSChar *attr_name);

#endif /* _VTDTOOLS_H */