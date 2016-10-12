/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 2DE06E
 * Name: SSDLNodeCheck_Valid
 * Desc: LNode索引是否有效
 * Ref: 一般规范
 */

#ifndef _SSDLNodeCheck_Valid_H
#define _SSDLNodeCheck_Valid_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDLNodeCheck_Valid : public CSingleInstance<CSSDLNodeCheck_Valid>, CNoCopy, public CCheckItemBase
{
public:
    CSSDLNodeCheck_Valid();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDLNodeCheck_Valid_H */
