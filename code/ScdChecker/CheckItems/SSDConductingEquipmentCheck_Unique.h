/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 07E93F
 * Name: SSDConductingEquipmentCheck_Unique
 * Desc: ConductingEquipment��ͬһ������Ƿ�Ψһ
 * Ref: �����ܱ��վSSD��ģ�����淶��6.5.2.a��
 */

#ifndef _SSDConductingEquipmentCheck_Unique_H
#define _SSDConductingEquipmentCheck_Unique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConductingEquipmentCheck_Unique : public CSingleInstance<CSSDConductingEquipmentCheck_Unique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConductingEquipmentCheck_Unique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConductingEquipmentCheck_Unique_H */
