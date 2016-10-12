/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 145768
 * Name: EnumtypeCheck
 * Desc: 检查标准EnumType的类型和值是否正确
 * Ref: GDW1396
 */

#ifndef _EnumtypeCheck_H
#define _EnumtypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CEnumtypeCheck : public CSingleInstance<CEnumtypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CEnumtypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _EnumtypeCheck_H */
