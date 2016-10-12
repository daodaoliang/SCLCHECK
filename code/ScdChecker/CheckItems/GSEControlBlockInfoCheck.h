/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: AE901A
 * Name: GSEControlBlockInfoCheck
 * Desc: GSE控制块关联属性GOID（appID）、APPID、MAC地址、VLAN-ID、优先级、MaxTime、MinTime存在缺失项
 * Ref: GDW1396
 */

#ifndef _GSEControlBlockInfoCheck_H
#define _GSEControlBlockInfoCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGSEControlBlockInfoCheck : public CSingleInstance<CGSEControlBlockInfoCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGSEControlBlockInfoCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GSEControlBlockInfoCheck_H */
