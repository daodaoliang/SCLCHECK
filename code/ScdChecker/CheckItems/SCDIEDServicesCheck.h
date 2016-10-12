/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 30D5AC
 * Name: SCDIEDServicesCheck
 * Desc: Servises元素是否符合规范
 * Ref: GDW1396
 */

#ifndef _SCDIEDServicesCheck_H
#define _SCDIEDServicesCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDIEDServicesCheck : public CSingleInstance<CSCDIEDServicesCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDIEDServicesCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDIEDServicesCheck_H */
