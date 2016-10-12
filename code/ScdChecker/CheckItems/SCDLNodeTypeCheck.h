/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 386A45
 * Name: SCDLNodeTypeCheck
 * Desc: LNodeType完整性检测
 * Ref: 智能变电站SCD审查技术规范
 */

#ifndef _SCDLNodeTypeCheck_H
#define _SCDLNodeTypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDLNodeTypeCheck : public CSingleInstance<CSCDLNodeTypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDLNodeTypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDLNodeTypeCheck_H */
