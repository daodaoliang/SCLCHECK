/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 509878
 * Name: SCDIEDAccessPointCheck
 * Desc: AccessPoint元素是否符合规范
 * Ref: GDW1396
 */

#ifndef _SCDIEDAccessPointCheck_H
#define _SCDIEDAccessPointCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDIEDAccessPointCheck : public CSingleInstance<CSCDIEDAccessPointCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDIEDAccessPointCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDIEDAccessPointCheck_H */
