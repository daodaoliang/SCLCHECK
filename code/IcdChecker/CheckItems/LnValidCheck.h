/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 0D52B6
 * Name: LnValidCheck
 * Desc: 逻辑节点的实例化建模原则检测，符合Q/GDW-1396标准
 * Ref: GDW1396
 */

#ifndef _LnValidCheck_H
#define _LnValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CLnValidCheck : public CSingleInstance<CLnValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLnValidCheck();

public:
    virtual bool CheckInternal();

    static void CheckValid_InCacheMgr(ICDCache& icd);
};

#endif /* _LnValidCheck_H */
