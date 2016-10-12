/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: F5EBE0
 * Name: SSDConductingEquipmentCheck
 * Desc: 导电设备ConductingEquipment命名是否符合标准；
 * Ref: 《智能变电站SSD建模技术规范》附录E.5
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
