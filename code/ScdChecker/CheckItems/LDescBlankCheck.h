/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 71A77B
 * Name: LDescBlankCheck
 * Desc: LD和LN实例的“desc”属性值是否非空
 * Ref: GDW1396
 */

#ifndef _LDescBlankCheck_H
#define _LDescBlankCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CLDescBlankCheck : public CSingleInstance<CLDescBlankCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLDescBlankCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _LDescBlankCheck_H */
