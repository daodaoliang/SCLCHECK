/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: E51D6C
 * Name: AppidUniqueCheck
 * Desc: Communication下SMV和GSE中的APPID是否全局唯一
 * Ref: GDW1396
 */

#ifndef _AppidUniqueCheck_H
#define _AppidUniqueCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CAppidUniqueCheck : public CSingleInstance<CAppidUniqueCheck>, CNoCopy, public CCheckItemBase
{
public:
    CAppidUniqueCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _AppidUniqueCheck_H */
