/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 7BCC9B
 * Name: SSDTerminalCheck_Valid
 * Desc: Terminal的SubstationName、VoltageLevelName、bayName、cNodeName索引是否有效
 * Ref: 一般要求
 */

#ifndef _SSDTerminalCheck_Valid_H
#define _SSDTerminalCheck_Valid_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDTerminalCheck_Valid : public CSingleInstance<CSSDTerminalCheck_Valid>, CNoCopy, public CCheckItemBase
{
public:
    CSSDTerminalCheck_Valid();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDTerminalCheck_Valid_H */
