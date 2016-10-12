/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: D66C1B
 * Name: SVControlBlockInfoCheck
 * Desc: SV控制块关联属性smvID、APPID、MAC地址、VLAN-ID存在缺失项
 * Ref: GDW1396
 */

#ifndef _SVControlBlockInfoCheck_H
#define _SVControlBlockInfoCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSVControlBlockInfoCheck : public CSingleInstance<CSVControlBlockInfoCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSVControlBlockInfoCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SVControlBlockInfoCheck_H */
