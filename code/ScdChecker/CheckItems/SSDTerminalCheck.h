/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 50B26C
 * Name: SSDTerminalCheck
 * Desc: 设备端点Terminal对象是否符合标准
 * Ref: 《智能变电站SSD建模技术规范》附录E.4
 */

#ifndef _SSDTerminalCheck_H
#define _SSDTerminalCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDTerminalCheck : public CSingleInstance<CSSDTerminalCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDTerminalCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDTerminalCheck_H */
