/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: C7C755
 * Name: ControlBlockLdCheck
 * Desc: 控制块关联数据集是否在同一LD中找到
 * Ref: GDW1396
 */

#ifndef _ControlBlockLdCheck_H
#define _ControlBlockLdCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CControlBlockLdCheck : public CSingleInstance<CControlBlockLdCheck>, CNoCopy, public CCheckItemBase
{
public:
    CControlBlockLdCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _ControlBlockLdCheck_H */
