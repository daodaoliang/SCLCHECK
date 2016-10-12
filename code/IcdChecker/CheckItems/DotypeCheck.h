/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 3E1AE0
 * Name: DotypeCheck
 * Desc: 对于引用了Q/GDW-1396标准中定义的数据对象类型（DOType），应与标准中定义的内容一致。(检查DOType的标准DA元素、SDO子元素顺序是否一致；检查DO Type的数据类型是否正确)
 * Ref: GDW1396
 */

#ifndef _DotypeCheck_H
#define _DotypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../../ScdCheckCommon/ValidateCache.h"

class CDotypeCheck : public CSingleInstance<CDotypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDotypeCheck();

public:
    virtual bool CheckInternal();

private:
    void CheckDOType(DOTypeCache& instDOType);
};

#endif /* _DotypeCheck_H */
