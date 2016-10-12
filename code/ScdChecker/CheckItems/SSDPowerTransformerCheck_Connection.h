/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: BD00E3
 * Name: SSDPowerTransformerCheck_Connection
 * Desc: PowerTransformer是否关联到Substation和VoltageLevel
 * Ref: 一般要求
 */

#ifndef _SSDPowerTransformerCheck_Connection_H
#define _SSDPowerTransformerCheck_Connection_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDPowerTransformerCheck_Connection : public CSingleInstance<CSSDPowerTransformerCheck_Connection>, CNoCopy, public CCheckItemBase
{
public:
    CSSDPowerTransformerCheck_Connection();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDPowerTransformerCheck_Connection_H */
