/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 0A9655
 * Name: SSDPowerTransformerCheck_Unique
 * Desc: PowerTransformer�Ƿ�ȫվΨһ
 * Ref: һ��Ҫ��
 */

#ifndef _SSDPowerTransformerCheck_Unique_H
#define _SSDPowerTransformerCheck_Unique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDPowerTransformerCheck_Unique : public CSingleInstance<CSSDPowerTransformerCheck_Unique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDPowerTransformerCheck_Unique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDPowerTransformerCheck_Unique_H */
