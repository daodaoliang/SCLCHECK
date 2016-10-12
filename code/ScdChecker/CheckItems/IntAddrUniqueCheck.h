/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: F6163B
 * Name: IntAddrUniqueCheck
 * Desc: GOOSE ��SV���õ�ExtRef�ڲ�intAddr��ֵ��IED��Χ���Ƿ�Ψһ��
 * Ref: GDW1396
 */

#ifndef _IntAddrUniqueCheck_H
#define _IntAddrUniqueCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CIntAddrUniqueCheck : public CSingleInstance<CIntAddrUniqueCheck>, CNoCopy, public CCheckItemBase
{
public:
    CIntAddrUniqueCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _IntAddrUniqueCheck_H */
