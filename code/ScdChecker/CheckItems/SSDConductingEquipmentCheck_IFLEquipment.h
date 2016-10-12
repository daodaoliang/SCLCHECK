/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 088D84
 * Name: SSDConductingEquipmentCheck_IFLEquipment
 * Desc: 引出线IFL类设备对象name是否符合标准；
 * Ref: 《智能变电站SSD建模技术规范》6.5.7.b）
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
