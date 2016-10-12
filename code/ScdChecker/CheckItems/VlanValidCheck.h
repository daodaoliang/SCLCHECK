/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: EA5BBA
 * Name: VlanValidCheck
 * Desc: Communication下SMV和GSE的VLAN-ID是否为3位16进制值，VLAN-Priority是否在0~7内
 * Ref: GDW1396
 */

#ifndef _VlanValidCheck_H
#define _VlanValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CVlanValidCheck : public CSingleInstance<CVlanValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CVlanValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _VlanValidCheck_H */
