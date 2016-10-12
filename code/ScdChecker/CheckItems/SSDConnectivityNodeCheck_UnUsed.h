/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: A897DD
 * Name: SSDConnectivityNodeCheck_UnUsed
 * Desc: ConnectivityNode未与Terminal关联
 * Ref: 《智能变电站SCD审查技术规范》7.2.3
 */

#ifndef _SSDConnectivityNodeCheck_UnUsed_H
#define _SSDConnectivityNodeCheck_UnUsed_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConnectivityNodeCheck_UnUsed : public CSingleInstance<CSSDConnectivityNodeCheck_UnUsed>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConnectivityNodeCheck_UnUsed();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConnectivityNodeCheck_UnUsed_H */
