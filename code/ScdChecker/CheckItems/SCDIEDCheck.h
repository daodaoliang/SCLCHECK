/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 69FBFF
 * Name: SCDIEDCheck
 * Desc: IED元素是否符合规范
 * Ref: GDW1396
 */

#ifndef _SCDIEDCheck_H
#define _SCDIEDCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDIEDCheck : public CSingleInstance<CSCDIEDCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDIEDCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDIEDCheck_H */
