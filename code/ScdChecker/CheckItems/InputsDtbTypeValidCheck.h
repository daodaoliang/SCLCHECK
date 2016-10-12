/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: F68BD2
 * Name: InputsDtbTypeValidCheck
 * Desc: SCD文件中虚端子连线的对象数据类型收发双方bType是否一致；
 * Ref: GDW1396
 */

#ifndef _InputsDtbTypeValidCheck_H
#define _InputsDtbTypeValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDtbTypeValidCheck : public CSingleInstance<CInputsDtbTypeValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDtbTypeValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDtbTypeValidCheck_H */
