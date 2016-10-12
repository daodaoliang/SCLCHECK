/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 71D964
 * Name: SCDDATypeCheck
 * Desc: DAType完整性检测
 * Ref: 智能变电站SCD审查技术规范
 */

#ifndef _SCDDATypeCheck_H
#define _SCDDATypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDDATypeCheck : public CSingleInstance<CSCDDATypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDDATypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDDATypeCheck_H */
