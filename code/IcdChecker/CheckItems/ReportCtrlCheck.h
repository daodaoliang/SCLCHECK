/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 8E9CE7
 * Name: ReportCtrlCheck
 * Desc: ������ƿ������Ӧ����Q/GDW-1396��׼�ж��������ԭ��
 * Ref: Q/GDW 1396-2012 ��8.3.2��
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
