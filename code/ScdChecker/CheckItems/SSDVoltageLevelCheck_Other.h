/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 6AD663
 * Name: SSDVoltageLevelCheck_Other
 * Desc: VoltageLevel是否含有其他元素（LNode等）
 * Ref: 《智能变电站SSD建模技术规范》6.3.b）
 */

#ifndef _SSDVoltageLevelCheck_Other_H
#define _SSDVoltageLevelCheck_Other_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck_Other : public CSingleInstance<CSSDVoltageLevelCheck_Other>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck_Other();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_Other_H */
