/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 8A3C65
 * Name: SSDLNodeCheck_Unique
 * Desc: LNode是否全站唯一
 * Ref: 一般规范
 */

#ifndef _SSDLNodeCheck_Unique_H
#define _SSDLNodeCheck_Unique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDLNodeCheck_Unique : public CSingleInstance<CSSDLNodeCheck_Unique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDLNodeCheck_Unique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDLNodeCheck_Unique_H */
