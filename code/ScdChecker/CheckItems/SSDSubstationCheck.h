/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 970E2B
 * Name: SSDSubstationCheck
 * Desc: SubStationԪ���Ƿ���Ϲ淶
 * Ref: �����ܱ��վSSD��ģ�����淶��
 */

#ifndef _SSDSubstationCheck_H
#define _SSDSubstationCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDSubstationCheck : public CSingleInstance<CSSDSubstationCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDSubstationCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDSubstationCheck_H */
