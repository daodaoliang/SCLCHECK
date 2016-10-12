/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 3AFF97
 * Name: SSDBayCheck_LineBay
 * Desc: 线路间隔是否含有引出线设备
 * Ref: 《智能变电站SSD建模技术规范》6.4.3.e)
 */

#ifndef _SSDBayCheck_LineBay_H
#define _SSDBayCheck_LineBay_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDBayCheck_LineBay : public CSingleInstance<CSSDBayCheck_LineBay>, CNoCopy, public CCheckItemBase
{
public:
    CSSDBayCheck_LineBay();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDBayCheck_LineBay_H */
