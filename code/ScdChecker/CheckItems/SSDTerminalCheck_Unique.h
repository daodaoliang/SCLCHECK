/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: A27269
 * Name: SSDTerminalCheck_Unique
 * Desc: Terminal命名装置内是否唯一
 * Ref: 一般要求
 */

#ifndef _SSDTerminalCheck_Unique_H
#define _SSDTerminalCheck_Unique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDTerminalCheck_Unique : public CSingleInstance<CSSDTerminalCheck_Unique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDTerminalCheck_Unique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDTerminalCheck_Unique_H */
