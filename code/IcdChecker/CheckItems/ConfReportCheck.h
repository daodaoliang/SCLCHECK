/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 0CB373
 * Name: ConfReportCheck
 * Desc: ������ƿ�ReportControlʵ�ʸ����Ƿ�С�ڻ����ConfReportControl��max����ֵ
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
