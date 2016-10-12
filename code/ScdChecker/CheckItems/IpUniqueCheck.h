/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 13FDFB
 * Name: IpUniqueCheck
 * Desc: SCD文件中IED的IP地址是否全局唯一
 * Ref: GDW1396
 */

#ifndef _IpUniqueCheck_H
#define _IpUniqueCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CIpUniqueCheck : public CSingleInstance<CIpUniqueCheck>, CNoCopy, public CCheckItemBase
{
public:
    CIpUniqueCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _IpUniqueCheck_H */
