/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 5A551E
 * Name: InputsDescValid1Check
 * Desc: 如有区分，跳闸信号用于启动失灵，启动失灵信号用于跳闸
 * Ref: GDW1396
 */

#ifndef _InputsDescValid1Check_H
#define _InputsDescValid1Check_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDescValid1Check : public CSingleInstance<CInputsDescValid1Check>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDescValid1Check();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDescValid1Check_H */
