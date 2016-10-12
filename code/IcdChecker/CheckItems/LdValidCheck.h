/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 6A2E19
 * Name: LdValidCheck
 * Desc: LDevice的命名应符合Q/GDW-1396标准中定义的命名原则
 * Ref: Q/GDW 1396-2012 第7.1.3节
 */

#ifndef _LdValidCheck_H
#define _LdValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CLdValidCheck : public CSingleInstance<CLdValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLdValidCheck();

public:
    virtual bool CheckInternal();

    void CheckName_InCacheMgr(std::wstring sName, int offset);

};

#endif /* _LdValidCheck_H */
