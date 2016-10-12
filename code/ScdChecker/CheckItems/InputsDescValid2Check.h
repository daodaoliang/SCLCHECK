/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 189315
 * Name: InputsDescValid2Check
 * Desc: 采样相位连接错误，跳闸启动失灵相位连接错误
 * Ref: GDW1396
 */

#ifndef _InputsDescValid2Check_H
#define _InputsDescValid2Check_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDescValid2Check : public CSingleInstance<CInputsDescValid2Check>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDescValid2Check();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDescValid2Check_H */
