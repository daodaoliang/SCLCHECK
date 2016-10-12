/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 5E04BF
 * Name: GeneralWarnCheck
 * Desc: 常见警告检查
 * Ref: GDW1396
 */

#ifndef _GeneralWarnCheck_H
#define _GeneralWarnCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGeneralWarnCheck : public CSingleInstance<CGeneralWarnCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGeneralWarnCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GeneralWarnCheck_H */
