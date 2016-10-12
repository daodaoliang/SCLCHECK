/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 661141
 * Name: LdDescCheck
 * Desc: LD和LN实例的“desc”属性值是否非空
 * Ref: Q/GDW 1396-2012 第6.2节
 */

#ifndef _LdDescCheck_H
#define _LdDescCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CLdDescCheck : public CSingleInstance<CLdDescCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLdDescCheck();

public:
    virtual bool CheckInternal();

    void CheckLNDesc_InCacheMgr(std::wstring sDesc, int offset);
    void CheckLDeviceDesc_InCacheMgr(LDeviceCache& device, int offset);
};

#endif /* _LdDescCheck_H */
