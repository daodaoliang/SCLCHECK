/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 8FE372
 * Name: DaTempValidCheck
 * Desc: 实例化配置DOI/SDI/DAI的对象索引是否与数据类型模板定义的数据对象结构一致
 * Ref: GDW1396
 */

#ifndef _DaTempValidCheck_H
#define _DaTempValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CDaTempValidCheck : public CSingleInstance<CDaTempValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDaTempValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _DaTempValidCheck_H */
