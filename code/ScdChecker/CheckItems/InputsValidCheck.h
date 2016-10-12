/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 39B6E1
 * Name: InputsValidCheck
 * Desc: SCD文件中虚端子连线的对象索引是否有效；
 * Ref: GDW1396
 */

#ifndef _InputsValidCheck_H
#define _InputsValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsValidCheck : public CSingleInstance<CInputsValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsValidCheck_H */
