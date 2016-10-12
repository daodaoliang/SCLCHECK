/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: EA5BBA
 * Name: VlanValidCheck
 * Desc: Communication��SMV��GSE��VLAN-ID�Ƿ�Ϊ3λ16����ֵ��VLAN-Priority�Ƿ���0~7��
 * Ref: GDW1396
 */

#ifndef _VlanValidCheck_H
#define _VlanValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CVlanValidCheck : public CSingleInstance<CVlanValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CVlanValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _VlanValidCheck_H */
