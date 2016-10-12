/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: D400F3
 * Name: SubFormatCheck
 * Desc: Communication下SubNetwork的子网命名是否符合规范中的命名要求；
 * Ref: GDW1396
 */

#ifndef _SubFormatCheck_H
#define _SubFormatCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSubFormatCheck : public CSingleInstance<CSubFormatCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSubFormatCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SubFormatCheck_H */
