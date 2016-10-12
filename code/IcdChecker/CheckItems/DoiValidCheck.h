/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 28F210
 * Name: DoiValidCheck
 * Desc: 实例化DOI元素的“desc”属性值与“dU” 属性值是否非空且两者一致
 * Ref: Q/GDW 1396-2012 第6.2节
 */

#ifndef _DoiValidCheck_H
#define _DoiValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"
class CDoiValidCheck : public CSingleInstance<CDoiValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDoiValidCheck();

public:
    virtual bool CheckInternal();

    void CheckDescAnddU_InCacheMgr(LNCache& ln, DOICache& doi, std::wstring sdU, int offset);
};

#endif /* _DoiValidCheck_H */
