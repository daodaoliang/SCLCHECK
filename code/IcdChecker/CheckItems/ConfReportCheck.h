/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 0CB373
 * Name: ConfReportCheck
 * Desc: 报告控制块ReportControl实际个数是否小于或等于ConfReportControl的max属性值
 * Ref: GDW1396
 */

#ifndef _ConfReportCheck_H
#define _ConfReportCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"
class CConfReportCheck : public CSingleInstance<CConfReportCheck>, CNoCopy, public CCheckItemBase
{
public:
    CConfReportCheck();

public:
    virtual bool CheckInternal();

    static void CheckReportCB_InCacheMgr(ICDCache& icd);
};

#endif /* _ConfReportCheck_H */
