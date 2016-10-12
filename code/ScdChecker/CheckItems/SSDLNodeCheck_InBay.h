/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 90F38A
 * Name: SSDLNodeCheck_InBay
 * Desc: LNode是否都在间隔内
 * Ref: 《智能变电站SSD建模技术规范》5.4
 */

#ifndef _SSDLNodeCheck_InBay_H
#define _SSDLNodeCheck_InBay_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDLNodeCheck_InBay : public CSingleInstance<CSSDLNodeCheck_InBay>, CNoCopy, public CCheckItemBase
{
public:
    CSSDLNodeCheck_InBay();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDLNodeCheck_InBay_H */
