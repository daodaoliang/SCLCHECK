/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 795860
 * Name: InputsDtValidCheck
 * Desc: SCD文件中虚端子连线的对象数据类型收发双方是否一致或兼容；
 * Ref: GDW1396
 */

#ifndef _InputsDtValidCheck_H
#define _InputsDtValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDtValidCheck : public CSingleInstance<CInputsDtValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDtValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDtValidCheck_H */
