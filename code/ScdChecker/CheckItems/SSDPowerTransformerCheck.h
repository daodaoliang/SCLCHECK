/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: D07CE2
 * Name: SSDPowerTransformerCheck
 * Desc: 变压器元素PowerTransformer是否符合规范
 * Ref: 《智能变电站SSD建模技术规范》附录E.4
 */

#ifndef _SSDPowerTransformerCheck_H
#define _SSDPowerTransformerCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDPowerTransformerCheck : public CSingleInstance<CSSDPowerTransformerCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDPowerTransformerCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDPowerTransformerCheck_H */
