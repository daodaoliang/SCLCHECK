/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 7F77BD
 * Name: SSDTerminalCheck_CNodeConnection
 * Desc: Terminal是否和ConnectivityNode相连
 * Ref: 《智能变电站SSD建模技术规范》5.3.c）
 */

#ifndef _SSDTerminalCheck_CNodeConnection_H
#define _SSDTerminalCheck_CNodeConnection_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDTerminalCheck_CNodeConnection : public CSingleInstance<CSSDTerminalCheck_CNodeConnection>, CNoCopy, public CCheckItemBase
{
public:
    CSSDTerminalCheck_CNodeConnection();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDTerminalCheck_CNodeConnection_H */
