/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 40F963
 * Name: SCDIEDLNCheck
 * Desc: LN元素是否符合规范
 * Ref: GDW1396
 */

#ifndef _SCDIEDLNCheck_H
#define _SCDIEDLNCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDIEDLNCheck : public CSingleInstance<CSCDIEDLNCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDIEDLNCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDIEDLNCheck_H */
