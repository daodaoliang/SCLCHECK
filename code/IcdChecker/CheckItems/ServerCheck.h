/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 9A2FE1
 * Name: ServerCheck
 * Desc: Server对象中至少应包含一个LDevice对象，每个LDevice对象至少应包含LLN0及LPHD两个逻辑节点。
 * Ref: Q/GDW 1396-2012 第7.1.1节
 */

#ifndef _ServerCheck_H
#define _ServerCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CServerCheck : public CSingleInstance<CServerCheck>, CNoCopy, public CCheckItemBase
{
public:
    CServerCheck();

public:
    virtual bool CheckInternal();

    void CheckLLN0LPHD_InCacheMgr(LDeviceCache& lDevice);
    void CheckhasLDevice_InCacheMgr(bool bHasLDevice, int offset);
};

#endif /* _ServerCheck_H */
