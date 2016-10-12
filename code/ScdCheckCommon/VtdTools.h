// 不对外开放

#ifndef _VTDTOOLS_H
#define _VTDTOOLS_H

#include "../VtdXml/everything.h"
#include <string>
#include <sstream>

// todo：此处在头文件中引入了命名空间，是否有必要移除？
using namespace com_ximpleware;

// 陆腾，导航器自动管理组件
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

// 陆腾，获取导航器中指定id的字符串
std::wstring VtdGetString(VTDNav *vn, int i);

// 陆腾，获取导航器中当前位置指定名称的属性值
std::wstring VtdGetAttributeValue(VTDNav *vn, const wchar_t *lpAttributeName);

// zxc 获取属性数量
int VtdGetAttributeCount(VTDNav *vn);

// zxc 获取当前Element的名称
std::wstring VtdGetElementName(VTDNav *vn);

// 陆腾，获取导航器中当前位置对应的字符串
std::wstring VtdGetTextValue(VTDNav *vn);

// 丛强
std::string GetXmlAttrVal(VTDNav *vn, const UCSChar *attr_name);

#endif /* _VTDTOOLS_H */