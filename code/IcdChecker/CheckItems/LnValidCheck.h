/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 0D52B6
 * Name: LnValidCheck
 * Desc: �߼��ڵ��ʵ������ģԭ���⣬����Q/GDW-1396��׼
 * Ref: GDW1396
 */

#ifndef _LnValidCheck_H
#define _LnValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CLnValidCheck : public CSingleInstance<CLnValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLnValidCheck();

public:
    virtual bool CheckInternal();

    static void CheckValid_InCacheMgr(ICDCache& icd);
};

#endif /* _LnValidCheck_H */
