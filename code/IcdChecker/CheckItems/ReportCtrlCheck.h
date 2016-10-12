/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 8E9CE7
 * Name: ReportCtrlCheck
 * Desc: 报告控制块的命名应符合Q/GDW-1396标准中定义的命名原则
 * Ref: Q/GDW 1396-2012 第8.3.2节
 */

#ifndef _ReportCtrlCheck_H
#define _ReportCtrlCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CReportCtrlCheck : public CSingleInstance<CReportCtrlCheck>, CNoCopy, public CCheckItemBase
{
public:
    CReportCtrlCheck();

public:
    virtual bool CheckInternal();

    static void CheckName_InCacheMgr(std::wstring sName, int offset);
};

#endif /* _ReportCtrlCheck_H */
