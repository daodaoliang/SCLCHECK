/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: FAD0DE
 * Name: SSDSubstationCheck_Normative
 * Desc: SubStation�����Ƿ���ϵ��ȹ淶
 * Ref: �����ܱ��վSSD��ģ�����淶��6.2
 */

#ifndef _SSDSubstationCheck_Normative_H
#define _SSDSubstationCheck_Normative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDSubstationCheck_Normative : public CSingleInstance<CSSDSubstationCheck_Normative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDSubstationCheck_Normative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDSubstationCheck_Normative_H */
