/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: E0B8B6
 * Name: SSDSubstationCheck_Unique
 * Desc: SubStation�Ƿ�Ψһ
 * Ref: �����ܱ��վSSD��ģ�����淶��6.2.a��
 */

#ifndef _SSDSubstationCheck_Unique_H
#define _SSDSubstationCheck_Unique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDSubstationCheck_Unique : public CSingleInstance<CSSDSubstationCheck_Unique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDSubstationCheck_Unique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDSubstationCheck_Unique_H */
