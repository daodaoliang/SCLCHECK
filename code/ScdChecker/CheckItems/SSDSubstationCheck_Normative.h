/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: FAD0DE
 * Name: SSDSubstationCheck_Normative
 * Desc: SubStation属性是否符合调度规范
 * Ref: 《智能变电站SSD建模技术规范》6.2
 */

#ifndef _SSDSubstationCheck_Normative_H
#define _SSDSubstationCheck_Normative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDSubstationCheck_Normative : public CSingleInstance<CSSDSubstationCheck_Normative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDSubstationCheck_Normative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDSubstationCheck_Normative_H */
