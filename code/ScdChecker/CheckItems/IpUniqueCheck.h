/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 13FDFB
 * Name: IpUniqueCheck
 * Desc: SCD�ļ���IED��IP��ַ�Ƿ�ȫ��Ψһ
 * Ref: GDW1396
 */

#ifndef _IpUniqueCheck_H
#define _IpUniqueCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CIpUniqueCheck : public CSingleInstance<CIpUniqueCheck>, CNoCopy, public CCheckItemBase
{
public:
    CIpUniqueCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _IpUniqueCheck_H */
