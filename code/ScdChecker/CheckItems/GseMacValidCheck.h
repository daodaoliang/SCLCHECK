/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 58E4DA
 * Name: GseMacValidCheck
 * Desc: GSE中MAC地址在01-0C-CD-01-00-00~01-0C-CD-01-FF-FF范围内
 * Ref: GDW1396
 */

#ifndef _GseMacValidCheck_H
#define _GseMacValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGseMacValidCheck : public CSingleInstance<CGseMacValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGseMacValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GseMacValidCheck_H */
