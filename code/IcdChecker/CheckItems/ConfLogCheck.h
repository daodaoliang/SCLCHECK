/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: B21624
 * Name: ConfLogCheck
 * Desc: 日志控制块LogControl实际个数是否小于或等于ConfLogControl的max属性值
 * Ref: GDW1396
 */

#ifndef _ConfLogCheck_H
#define _ConfLogCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CConfLogCheck : public CSingleInstance<CConfLogCheck>, CNoCopy, public CCheckItemBase
{
public:
    CConfLogCheck();

public:
    virtual bool CheckInternal();

    static void CheckLogCB_InCacheMgr(ICDCache& icd);
};

#endif /* _ConfLogCheck_H */
