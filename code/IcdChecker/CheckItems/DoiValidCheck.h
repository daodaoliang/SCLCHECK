/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 28F210
 * Name: DoiValidCheck
 * Desc: ʵ����DOIԪ�صġ�desc������ֵ�롰dU�� ����ֵ�Ƿ�ǿ�������һ��
 * Ref: Q/GDW 1396-2012 ��6.2��
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
