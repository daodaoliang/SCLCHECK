/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 20544B
 * Name: MLntypeCheck
 * Desc: 检查标准LNodeType的强制（或条件强制）DO是否存在
 * Ref: Q/GDW 1396-2012 第7.1.7节
 */

#ifndef _MLntypeCheck_H
#define _MLntypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

#include "..\..\ScdCheckCommon\ValidateCache.h"

class CMLntypeCheck : public CSingleInstance<CMLntypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CMLntypeCheck();

public:
    virtual bool CheckInternal();

private:
    // 检查标准LNodeType的强制（或条件强制）DO是否存在
    void CheckMLNodeTypeDo(LNodeTypeCache& inst, LNodeTypeCache& temp);

};

#endif /* _MLntypeCheck_H */
