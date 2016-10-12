/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 3C04EC
 * Name: SSDBayCheck_dTypeNormative
 * Desc: 一次设备间隔Bay元素的扩展属性dType是否符合规范
 * Ref: 《智能变电站SSD建模技术规范》6.4.3.b)
 */

#ifndef _SSDBayCheck_dTypeNormative_H
#define _SSDBayCheck_dTypeNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDBayCheck_dTypeNormative : public CSingleInstance<CSSDBayCheck_dTypeNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDBayCheck_dTypeNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDBayCheck_dTypeNormative_H */
