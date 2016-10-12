/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: E58327
 * Name: InputsStadValidCheck
 * Desc: 各“六统一”保护该连接的信号没有连接或连接不正确
 * Ref: GDW1396
 */

#ifndef _InputsStadValidCheck_H
#define _InputsStadValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsStadValidCheck : public CSingleInstance<CInputsStadValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsStadValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsStadValidCheck_H */
