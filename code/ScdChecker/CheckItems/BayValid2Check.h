/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: EB1230
 * Name: BayValid2Check
 * Desc: 母差内支路合并单元与智能终端不一致错误
 * Ref: GDW1396
 */

#ifndef _BayValid2Check_H
#define _BayValid2Check_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../../ScdCheckCommon/ValidateCache.h"

class CBayValid2Check : public CSingleInstance<CBayValid2Check>, CNoCopy, public CCheckItemBase
{
public:
    CBayValid2Check();

public:
    virtual bool CheckInternal();

private:
    IntervalMap m_BranchIEDMap; // 记录支路与IED
    // IntervalMap m_BranchIntervalMap; // 记录支路与间隔
    wstring GetBranch(wstring &desc);
    wstring CBayValid2Check::GetIEDInterval(wstring iedName);
};

#endif /* _BayValid2Check_H */
