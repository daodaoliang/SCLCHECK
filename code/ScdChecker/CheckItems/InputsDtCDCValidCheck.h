/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 7652D1
 * Name: InputsDtCDCValidCheck
 * Desc: SCD文件中虚端子连线的对象数据类型收发双方CDC是否一致；
 * Ref: GDW1396
 */

#ifndef _InputsDtCDCValidCheck_H
#define _InputsDtCDCValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDtCDCValidCheck : public CSingleInstance<CInputsDtCDCValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDtCDCValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDtCDCValidCheck_H */
