/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 970E2B
 * Name: SSDSubstationCheck
 * Desc: SubStation元素是否符合规范
 * Ref: 《智能变电站SSD建模技术规范》
 */

#ifndef _SSDSubstationCheck_H
#define _SSDSubstationCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDSubstationCheck : public CSingleInstance<CSSDSubstationCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDSubstationCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDSubstationCheck_H */
