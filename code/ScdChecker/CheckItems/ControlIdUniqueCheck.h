/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 2E8E7B
 * Name: ControlIdUniqueCheck
 * Desc: SCD文件中GSEControl的appID的值、SampledValueControl的smvID的值是否全局唯一
 * Ref: GDW1396
 */

#ifndef _ControlIdUniqueCheck_H
#define _ControlIdUniqueCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CControlIdUniqueCheck : public CSingleInstance<CControlIdUniqueCheck>, CNoCopy, public CCheckItemBase
{
public:
    CControlIdUniqueCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _ControlIdUniqueCheck_H */
