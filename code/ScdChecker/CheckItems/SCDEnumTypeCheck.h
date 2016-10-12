/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 4BA204
 * Name: SCDEnumTypeCheck
 * Desc: EnumType完整性检测
 * Ref: 智能变电站SCD审查技术规范
 */

#ifndef _SCDEnumTypeCheck_H
#define _SCDEnumTypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDEnumTypeCheck : public CSingleInstance<CSCDEnumTypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDEnumTypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDEnumTypeCheck_H */
