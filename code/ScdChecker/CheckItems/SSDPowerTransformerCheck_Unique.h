/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 0A9655
 * Name: SSDPowerTransformerCheck_Unique
 * Desc: PowerTransformer是否全站唯一
 * Ref: 一般要求
 */

#ifndef _SSDPowerTransformerCheck_Unique_H
#define _SSDPowerTransformerCheck_Unique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDPowerTransformerCheck_Unique : public CSingleInstance<CSSDPowerTransformerCheck_Unique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDPowerTransformerCheck_Unique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDPowerTransformerCheck_Unique_H */
