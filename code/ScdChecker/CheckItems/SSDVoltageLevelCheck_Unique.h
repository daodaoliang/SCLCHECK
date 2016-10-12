/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: B179A8
 * Name: SSDVoltageLevelCheck_Unique
 * Desc: VoltageLevel是否唯一
 * Ref: 《智能变电站SSD建模技术规范》6.3.b)
 */

#ifndef _SSDVoltageLevelCheck_Unique_H
#define _SSDVoltageLevelCheck_Unique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck_Unique : public CSingleInstance<CSSDVoltageLevelCheck_Unique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck_Unique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_Unique_H */
