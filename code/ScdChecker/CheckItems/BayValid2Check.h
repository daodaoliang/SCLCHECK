/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: EB1230
 * Name: BayValid2Check
 * Desc: ĸ����֧·�ϲ���Ԫ�������ն˲�һ�´���
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
    IntervalMap m_BranchIEDMap; // ��¼֧·��IED
    // IntervalMap m_BranchIntervalMap; // ��¼֧·����
    wstring GetBranch(wstring &desc);
    wstring CBayValid2Check::GetIEDInterval(wstring iedName);
};

#endif /* _BayValid2Check_H */
