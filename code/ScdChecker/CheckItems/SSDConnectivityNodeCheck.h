/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 7BDF46
 * Name: SSDConnectivityNodeCheck
 * Desc: ConnectivityNode是否符合国网标准；
 * Ref: 《智能变电站SSD建模技术规范》附录E.6
 */

#ifndef _SSDConnectivityNodeCheck_H
#define _SSDConnectivityNodeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConnectivityNodeCheck : public CSingleInstance<CSSDConnectivityNodeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConnectivityNodeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConnectivityNodeCheck_H */
