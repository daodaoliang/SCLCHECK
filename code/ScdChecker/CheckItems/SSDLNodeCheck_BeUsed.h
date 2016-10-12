/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: D6F36C
 * Name: SSDLNodeCheck_BeUsed
 * Desc: LNode是否只被引用一次
 * Ref: 一般规范
 */

#ifndef _SSDLNodeCheck_BeUsed_H
#define _SSDLNodeCheck_BeUsed_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDLNodeCheck_BeUsed : public CSingleInstance<CSSDLNodeCheck_BeUsed>, CNoCopy, public CCheckItemBase
{
public:
    CSSDLNodeCheck_BeUsed();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDLNodeCheck_BeUsed_H */
