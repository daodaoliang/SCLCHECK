/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 296AE3
 * Name: SCDDOTypeCheck
 * Desc: DOType完整性检测
 * Ref: 智能变电站SCD审查技术规范
 */

#ifndef _SCDDOTypeCheck_H
#define _SCDDOTypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDDOTypeCheck : public CSingleInstance<CSCDDOTypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDDOTypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDDOTypeCheck_H */
