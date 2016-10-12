/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: D7FF46
 * Name: SSDConductingEquipmentCheck_Normative
 * Desc: �����豸ConductingEquipment�����Ƿ���Ϲ淶��
 * Ref: �����ܱ��վSSD��ģ�����淶��6.5.2.b��
 */

#ifndef _SSDConductingEquipmentCheck_Normative_H
#define _SSDConductingEquipmentCheck_Normative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConductingEquipmentCheck_Normative : public CSingleInstance<CSSDConductingEquipmentCheck_Normative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConductingEquipmentCheck_Normative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConductingEquipmentCheck_Normative_H */
