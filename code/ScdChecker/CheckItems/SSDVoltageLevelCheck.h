/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 8F38E4
 * Name: SSDVoltageLevelCheck
 * Desc: VoltageLevel元素是否符合规范；
 * Ref: 《智能变电站SSD建模技术规范》附录E.2
 */

#ifndef _SSDVoltageLevelCheck_H
#define _SSDVoltageLevelCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck : public CSingleInstance<CSSDVoltageLevelCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_H */
