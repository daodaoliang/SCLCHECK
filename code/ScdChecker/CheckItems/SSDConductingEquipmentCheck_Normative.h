/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: D7FF46
 * Name: SSDConductingEquipmentCheck_Normative
 * Desc: 导电设备ConductingEquipment命名是否符合规范；
 * Ref: 《智能变电站SSD建模技术规范》6.5.2.b）
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
