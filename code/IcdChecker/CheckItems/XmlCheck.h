/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: CE18E6
 * Name: XmlCheck
 * Desc: XML格式规范
 * Ref: GDW1396
 */

#ifndef _XmlCheck_H
#define _XmlCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CXmlCheck : public CSingleInstance<CXmlCheck>, CNoCopy, public CCheckItemBase
{
public:
    CXmlCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _XmlCheck_H */
