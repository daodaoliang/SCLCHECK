/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 8E9CE7
 * Name: MacAddrUniqueCheck
 * Desc: SCD文件中Communication下MAC-Address的值是否全局唯一
 * Ref: GDW1396
 */

#ifndef _MacAddrUniqueCheck_H
#define _MacAddrUniqueCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CMacAddrUniqueCheck : public CSingleInstance<CMacAddrUniqueCheck>, CNoCopy, public CCheckItemBase
{
public:
    CMacAddrUniqueCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _MacAddrUniqueCheck_H */
