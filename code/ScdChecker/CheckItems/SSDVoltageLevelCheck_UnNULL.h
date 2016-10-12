/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: E81083
 * Name: SSDVoltageLevelCheck_UnNULL
 * Desc: VoltageLevel为空/非空时，Voltage值是否为空非空
 * Ref: 《智能变电站SSD建模技术规范》6.3.d)
 */

#ifndef _SSDVoltageLevelCheck_UnNULL_H
#define _SSDVoltageLevelCheck_UnNULL_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck_UnNULL : public CSingleInstance<CSSDVoltageLevelCheck_UnNULL>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck_UnNULL();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_UnNULL_H */
