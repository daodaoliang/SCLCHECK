/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: A63491
 * Name: FcdaValidCheck
 * Desc: 数据集配置的FCDA非空的属性值是否指向模型中已存在的数据对象和数据属性
 * Ref: GDW1396
 */

#ifndef _FcdaValidCheck_H
#define _FcdaValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CFcdaValidCheck : public CSingleInstance<CFcdaValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CFcdaValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _FcdaValidCheck_H */
