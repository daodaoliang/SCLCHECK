/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 5EEDF0
 * Name: SSDBayCheck
 * Desc: Bay元素是否符合规范；
 * Ref: 《智能变电站SSD建模技术规范》附录E.3
 */

#ifndef _SSDBayCheck_H
#define _SSDBayCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDBayCheck : public CSingleInstance<CSSDBayCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDBayCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDBayCheck_H */
