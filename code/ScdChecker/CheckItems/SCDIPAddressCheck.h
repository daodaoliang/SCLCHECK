/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 25B22A
 * Name: SCDIPAddressCheck
 * Desc: IP所在Address元素检测
 * Ref: GDW1396
 */

#ifndef _SCDIPAddressCheck_H
#define _SCDIPAddressCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDIPAddressCheck : public CSingleInstance<CSCDIPAddressCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDIPAddressCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDIPAddressCheck_H */
