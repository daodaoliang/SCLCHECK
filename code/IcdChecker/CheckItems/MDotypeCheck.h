/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: BB4024
 * Name: MDotypeCheck
 * Desc: 检查标准DOType的强制DA元素是否存在
 * Ref: Q/GDW 1396-2012 第7.1.6节
 */

#ifndef _MDotypeCheck_H
#define _MDotypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../../ScdCheckCommon/ValidateCache.h"

class CMDotypeCheck : public CSingleInstance<CMDotypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CMDotypeCheck();

public:
    virtual bool CheckInternal();

private:
    // 检查标准DOType的强制DA元素是否存在
    void CheckMDOTypeDA(DOTypeCache& inst, DOTypeCache& temp);
};

#endif /* _MDotypeCheck_H */
