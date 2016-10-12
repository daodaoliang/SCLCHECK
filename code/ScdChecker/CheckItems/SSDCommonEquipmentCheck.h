/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: A7D962
 * Name: SSDCommonEquipmentCheck
 * Desc: 通用Equipment元素是否符合国网标准；
 * Ref: 《智能变电站SSD建模技术规范》
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
