/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: A7D962
 * Name: SSDCommonEquipmentCheck
 * Desc: ͨ��EquipmentԪ���Ƿ���Ϲ�����׼��
 * Ref: �����ܱ��վSSD��ģ�����淶��
 */

#ifndef _SSDCommonEquipmentCheck_H
#define _SSDCommonEquipmentCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDCommonEquipmentCheck : public CSingleInstance<CSSDCommonEquipmentCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDCommonEquipmentCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDCommonEquipmentCheck_H */
