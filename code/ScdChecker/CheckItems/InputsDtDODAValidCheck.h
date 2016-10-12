/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 795860
 * Name: InputsDtDODAValidCheck
 * Desc: SCD文件中虚端子连线的对象数据类型收发双方是否为DO/DA或DA/DO；
 * Ref: GDW1396
 */

#ifndef _InputsDtDODAValidCheck_H
#define _InputsDtDODAValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDtDODAValidCheck : public CSingleInstance<CInputsDtDODAValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDtDODAValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDtDODAValidCheck_H */
