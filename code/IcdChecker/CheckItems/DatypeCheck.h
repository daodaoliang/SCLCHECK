/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: DE1D56
 * Name: DatypeCheck
 * Desc: 对于引用了Q/GDW-1396标准中定义的数据属性类型（DAType），应与标准中定义的内容一致(检查DAType的BDA元素顺序是否一致，检查DAType的数据类型是否正确)
 * Ref: 工程实践（SCD检测已有）或参照检测标准
 */

#ifndef _DatypeCheck_H
#define _DatypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../../ScdCheckCommon/ValidateCache.h"

class CDatypeCheck : public CSingleInstance<CDatypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDatypeCheck();

public:
    virtual bool CheckInternal();

private:
    void CheckDAType(DATypeCache& instDA, DATypeCache& tempDA);
};

#endif /* _DatypeCheck_H */
