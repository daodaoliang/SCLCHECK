/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: C7C755
 * Name: ControlBlockLdCheck
 * Desc: ���ƿ�������ݼ��Ƿ���ͬһLD���ҵ�
 * Ref: GDW1396
 */

#ifndef _ControlBlockLdCheck_H
#define _ControlBlockLdCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CControlBlockLdCheck : public CSingleInstance<CControlBlockLdCheck>, CNoCopy, public CCheckItemBase
{
public:
    CControlBlockLdCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _ControlBlockLdCheck_H */
