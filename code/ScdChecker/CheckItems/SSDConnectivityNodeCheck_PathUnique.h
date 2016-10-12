/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: A897DD
 * Name: SSDConnectivityNodeCheck_PathUnique
 * Desc: ConnectivityNode的pathName是否全站唯一
 * Ref: 《智能变电站SCD审查技术规范》7.2.3
 */

#ifndef _SSDConnectivityNodeCheck_PathUnique_H
#define _SSDConnectivityNodeCheck_PathUnique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConnectivityNodeCheck_PathUnique : public CSingleInstance<CSSDConnectivityNodeCheck_PathUnique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConnectivityNodeCheck_PathUnique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConnectivityNodeCheck_PathUnique_H */
