/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: D07CE2
 * Name: SSDPowerTransformerCheck
 * Desc: ��ѹ��Ԫ��PowerTransformer�Ƿ���Ϲ淶
 * Ref: �����ܱ��վSSD��ģ�����淶����¼E.4
 */

#ifndef _SSDPowerTransformerCheck_H
#define _SSDPowerTransformerCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDPowerTransformerCheck : public CSingleInstance<CSSDPowerTransformerCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDPowerTransformerCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDPowerTransformerCheck_H */
