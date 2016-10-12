/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: E0B8B6
 * Name: SSDSubstationCheck_Unique
 * Desc: SubStation是否唯一
 * Ref: 《智能变电站SSD建模技术规范》6.2.a）
 */

#ifndef _SSDSubstationCheck_Unique_H
#define _SSDSubstationCheck_Unique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDSubstationCheck_Unique : public CSingleInstance<CSSDSubstationCheck_Unique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDSubstationCheck_Unique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDSubstationCheck_Unique_H */
