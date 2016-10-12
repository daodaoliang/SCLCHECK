/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: A4401A
 * Name: SSDBayCheck_AttrNormative
 * Desc: 一次设备间隔Bay元素的name、desc命名是否符合规范；
 * Ref: 《智能变电站SSD建模技术规范》6.4.3.a)
 */

#ifndef _SSDBayCheck_AttrNormative_H
#define _SSDBayCheck_AttrNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDBayCheck_AttrNormative : public CSingleInstance<CSSDBayCheck_AttrNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDBayCheck_AttrNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDBayCheck_AttrNormative_H */
