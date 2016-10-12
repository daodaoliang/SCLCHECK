/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 56AB89
 * Name: REnumtypeCheck
 * Desc: 检查是否存在扩展的EnumType
 * Ref: GDW1396
 */

#ifndef _REnumtypeCheck_H
#define _REnumtypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CREnumtypeCheck : public CSingleInstance<CREnumtypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CREnumtypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _REnumtypeCheck_H */
