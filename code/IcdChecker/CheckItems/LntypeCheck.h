/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 5BA640
 * Name: LntypeCheck
 * Desc: 标准LNodeType检查（检查标准LNodeType的标准DO顺序是否正确、检查标准LNodeType的标准DO的数据类型是否正确、标准LNodeType是否存在扩展的DO类型）
 * Ref: GDW1396
 */

#ifndef _LntypeCheck_H
#define _LntypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

#include "..\..\ScdCheckCommon\ValidateCache.h"

class CLntypeCheck : public CSingleInstance<CLntypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLntypeCheck();

public:
    virtual bool CheckInternal();

public:

private:
    // 检查DO的顺序、类型是否与标准模板中一致
    void CheckDO(LNodeTypeCache& inst, LNodeTypeCache& temp);
    
};

#endif /* _LntypeCheck_H */
