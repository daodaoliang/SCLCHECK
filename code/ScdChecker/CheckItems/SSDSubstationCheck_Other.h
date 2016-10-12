/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 894442
 * Name: SSDSubstationCheck_Other
 * Desc: SubStation是否含有其他元素（LNode等）
 * Ref: 《智能变电站SSD建模技术规范》6.2
 */

#ifndef _SSDSubstationCheck_Other_H
#define _SSDSubstationCheck_Other_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDSubstationCheck_Other : public CSingleInstance<CSSDSubstationCheck_Other>, CNoCopy, public CCheckItemBase
{
public:
    CSSDSubstationCheck_Other();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDSubstationCheck_Other_H */
