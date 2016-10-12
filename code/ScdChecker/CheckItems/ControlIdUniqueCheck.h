/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 2E8E7B
 * Name: ControlIdUniqueCheck
 * Desc: SCD�ļ���GSEControl��appID��ֵ��SampledValueControl��smvID��ֵ�Ƿ�ȫ��Ψһ
 * Ref: GDW1396
 */

#ifndef _ControlIdUniqueCheck_H
#define _ControlIdUniqueCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CControlIdUniqueCheck : public CSingleInstance<CControlIdUniqueCheck>, CNoCopy, public CCheckItemBase
{
public:
    CControlIdUniqueCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _ControlIdUniqueCheck_H */
