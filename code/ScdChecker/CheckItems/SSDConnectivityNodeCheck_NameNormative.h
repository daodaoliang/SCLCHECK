/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 6D6D0B
 * Name: SSDConnectivityNodeCheck_NameNormative
 * Desc: ConnectivityNode的name属性是否按照Cn进行命名
 * Ref: 《智能变电站SSD建模技术规范》6.5.5.a)
 */

#ifndef _SSDConnectivityNodeCheck_NameNormative_H
#define _SSDConnectivityNodeCheck_NameNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConnectivityNodeCheck_NameNormative : public CSingleInstance<CSSDConnectivityNodeCheck_NameNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConnectivityNodeCheck_NameNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConnectivityNodeCheck_NameNormative_H */
