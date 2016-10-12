/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 221E7A
 * Name: SSDVoltageLevelCheck_Normative
 * Desc: VoltageLevel的name、desc属性是否符合额定电压标准
 * Ref: 《智能变电站SSD建模技术规范》6.3.b）
 */

#ifndef _SSDVoltageLevelCheck_Normative_H
#define _SSDVoltageLevelCheck_Normative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck_Normative : public CSingleInstance<CSSDVoltageLevelCheck_Normative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck_Normative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_Normative_H */
