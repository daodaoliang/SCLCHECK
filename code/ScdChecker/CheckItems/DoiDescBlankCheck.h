/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: D25578
 * Name: DoiDescBlankCheck
 * Desc: 实例化DOI元素的“desc”属性值与“dU” 属性值是否非空且两者一致；
 * Ref: GDW1396
 */

#ifndef _DoiDescBlankCheck_H
#define _DoiDescBlankCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CDoiDescBlankCheck : public CSingleInstance<CDoiDescBlankCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDoiDescBlankCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _DoiDescBlankCheck_H */
