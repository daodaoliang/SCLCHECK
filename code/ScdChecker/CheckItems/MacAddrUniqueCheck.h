/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 8E9CE7
 * Name: MacAddrUniqueCheck
 * Desc: SCD�ļ���Communication��MAC-Address��ֵ�Ƿ�ȫ��Ψһ
 * Ref: GDW1396
 */

#ifndef _MacAddrUniqueCheck_H
#define _MacAddrUniqueCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CMacAddrUniqueCheck : public CSingleInstance<CMacAddrUniqueCheck>, CNoCopy, public CCheckItemBase
{
public:
    CMacAddrUniqueCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _MacAddrUniqueCheck_H */
