/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 894442
 * Name: SSDSubstationCheck_Other
 * Desc: SubStation�Ƿ�������Ԫ�أ�LNode�ȣ�
 * Ref: �����ܱ��վSSD��ģ�����淶��6.2
 */

#ifndef _SSDSubstationCheck_Other_H
#define _SSDSubstationCheck_Other_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDSubstationCheck_Other : public CSingleInstance<CSSDSubstationCheck_Other>, CNoCopy, public CCheckItemBase
{
public:
    CSSDSubstationCheck_Other();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDSubstationCheck_Other_H */
