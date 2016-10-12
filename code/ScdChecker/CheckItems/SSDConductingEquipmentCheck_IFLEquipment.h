/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 088D84
 * Name: SSDConductingEquipmentCheck_IFLEquipment
 * Desc: ������IFL���豸����name�Ƿ���ϱ�׼��
 * Ref: �����ܱ��վSSD��ģ�����淶��6.5.7.b��
 */

#ifndef _SSDConductingEquipmentCheck_IFLEquipment_H
#define _SSDConductingEquipmentCheck_IFLEquipment_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConductingEquipmentCheck_IFLEquipment : public CSingleInstance<CSSDConductingEquipmentCheck_IFLEquipment>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConductingEquipmentCheck_IFLEquipment();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConductingEquipmentCheck_IFLEquipment_H */
