/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: FBA7BB
 * Name: SSDLNodeCheck
 * Desc: LNode元素是否符合国网标准；
 * Ref: 《智能变电站SSD建模技术规范》附录E.3
 */

#ifndef _SSDLNodeCheck_H
#define _SSDLNodeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDLNodeCheck : public CSingleInstance<CSSDLNodeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDLNodeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDLNodeCheck_H */
