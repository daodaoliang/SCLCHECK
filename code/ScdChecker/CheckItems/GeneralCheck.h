/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 5E04BF
 * Name: GeneralCheck
 * Desc: 其他问题检查
 * Ref: GDW1396
 */

#ifndef _GeneralCheck_H
#define _GeneralCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGeneralCheck : public CSingleInstance<CGeneralCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGeneralCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GeneralCheck_H */
