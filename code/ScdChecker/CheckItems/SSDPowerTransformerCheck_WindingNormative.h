/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 6AD8BF
 * Name: SSDPowerTransformerCheck_WindingNormative
 * Desc: PowerTransformer�Ƿ��������������ѹ��������Ԫ��
 * Ref: �����ܱ��վSSD��ģ�����淶��6.5.1
 */

#ifndef _SSDPowerTransformerCheck_WindingNormative_H
#define _SSDPowerTransformerCheck_WindingNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDPowerTransformerCheck_WindingNormative : public CSingleInstance<CSSDPowerTransformerCheck_WindingNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDPowerTransformerCheck_WindingNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDPowerTransformerCheck_WindingNormative_H */
