/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 2C98FA
 * Name: RedLntypeCheck
 * Desc: 检查是否存在扩展的LNodeType，扩展的LNodeType应符合命名规范（制造厂商前缀_装置型号_版本号_LN类名(_其它后缀)），扩展LNodeType的命名空间要存在
 * Ref: Q/GDW 1396-2012 第7.1.7节
 */

#ifndef _RedLntypeCheck_H
#define _RedLntypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

#include "..\..\ScdCheckCommon\ValidateCache.h"

class CRedLntypeCheck : public CSingleInstance<CRedLntypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CRedLntypeCheck();

public:
    virtual bool CheckInternal();

private:
    void CheckExtendLNodeType(LNodeTypeCache& instLNodeType);
};

#endif /* _RedLntypeCheck_H */
