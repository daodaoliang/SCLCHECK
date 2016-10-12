/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 81DA52
 * Name: SCDHeaderCheck
 * Desc: Header元素是否符合规范
 * Ref: GDW1396
 */

#ifndef _SCDHeaderCheck_H
#define _SCDHeaderCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDHeaderCheck : public CSingleInstance<CSCDHeaderCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDHeaderCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDHeaderCheck_H */
