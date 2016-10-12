/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: B861B0
 * Name: GeneralErrorCheck
 * Desc: 常见错误检查
 * Ref: GDW1396
 */

#ifndef _GeneralErrorCheck_H
#define _GeneralErrorCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGeneralErrorCheck : public CSingleInstance<CGeneralErrorCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGeneralErrorCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GeneralErrorCheck_H */
