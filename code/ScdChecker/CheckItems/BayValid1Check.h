/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 06857F
 * Name: BayValid1Check
 * Desc: 保护、测控、合并单元、智能终端间隔连接错误
 * Ref: GDW1396
 */

#ifndef _BayValid1Check_H
#define _BayValid1Check_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CBayValid1Check : public CSingleInstance<CBayValid1Check>, CNoCopy, public CCheckItemBase
{
public:
    CBayValid1Check();

public:
    virtual bool CheckInternal();
};

#endif /* _BayValid1Check_H */
