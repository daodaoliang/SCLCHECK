/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: A63491
 * Name: RedDotypeCheck
 * Desc: 检查是否存在扩展的DO，扩展时应符合命名规范（制造厂商前缀_装置型号_版本号_DO类名(_其它后缀)）
 * Ref: Q/GDW 1396-2012 第7.1.6节
 */

#ifndef _RedDotypeCheck_H
#define _RedDotypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../../ScdCheckCommon/ValidateCache.h"

class CRedDotypeCheck : public CSingleInstance<CRedDotypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CRedDotypeCheck();

public:
    virtual bool CheckInternal();

private:
    void CheckDOType(DOTypeCache& instDOType);
};

#endif /* _RedDotypeCheck_H */
