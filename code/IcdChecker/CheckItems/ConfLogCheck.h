/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: B21624
 * Name: ConfLogCheck
 * Desc: ��־���ƿ�LogControlʵ�ʸ����Ƿ�С�ڻ����ConfLogControl��max����ֵ
 * Ref: GDW1396
 */

#ifndef _ConfLogCheck_H
#define _ConfLogCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CConfLogCheck : public CSingleInstance<CConfLogCheck>, CNoCopy, public CCheckItemBase
{
public:
    CConfLogCheck();

public:
    virtual bool CheckInternal();

    static void CheckLogCB_InCacheMgr(ICDCache& icd);
};

#endif /* _ConfLogCheck_H */
