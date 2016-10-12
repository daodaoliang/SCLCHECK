/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 6AD8BF
 * Name: SSDPowerTransformerCheck_WindingNormative
 * Desc: PowerTransformer是否存在至少两个变压器绕组子元素
 * Ref: 《智能变电站SSD建模技术规范》6.5.1
 */

#ifndef _SSDPowerTransformerCheck_WindingNormative_H
#define _SSDPowerTransformerCheck_WindingNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDPowerTransformerCheck_WindingNormative : public CSingleInstance<CSSDPowerTransformerCheck_WindingNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDPowerTransformerCheck_WindingNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDPowerTransformerCheck_WindingNormative_H */
