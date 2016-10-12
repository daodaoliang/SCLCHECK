/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 07E93F
 * Name: SSDConductingEquipmentCheck_Unique
 * Desc: ConductingEquipment在同一间隔内是否唯一
 * Ref: 《智能变电站SSD建模技术规范》6.5.2.a）
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
