/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: F5EBE0
 * Name: SSDConductingEquipmentCheck
 * Desc: �����豸ConductingEquipment�����Ƿ���ϱ�׼��
 * Ref: �����ܱ��վSSD��ģ�����淶����¼E.5
 */

#ifndef _SSDConductingEquipmentCheck_H
#define _SSDConductingEquipmentCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConductingEquipmentCheck : public CSingleInstance<CSSDConductingEquipmentCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConductingEquipmentCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConductingEquipmentCheck_H */
