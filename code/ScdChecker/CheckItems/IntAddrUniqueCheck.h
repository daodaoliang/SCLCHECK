/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: F6163B
 * Name: IntAddrUniqueCheck
 * Desc: GOOSE 和SV配置的ExtRef内部intAddr的值在IED范围内是否唯一；
 * Ref: GDW1396
 */

#ifndef _IntAddrUniqueCheck_H
#define _IntAddrUniqueCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CIntAddrUniqueCheck : public CSingleInstance<CIntAddrUniqueCheck>, CNoCopy, public CCheckItemBase
{
public:
    CIntAddrUniqueCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _IntAddrUniqueCheck_H */
