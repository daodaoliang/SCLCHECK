/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 7053C8
 * Name: IpValidCheck
 * Desc: IP地址、子网掩码的每一位在0~255范围内
 * Ref: GDW1396
 */

#ifndef _IpValidCheck_H
#define _IpValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CIpValidCheck : public CSingleInstance<CIpValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CIpValidCheck();

public:
    virtual bool CheckInternal();

    int StrToIntDef(const char *str, int default_value);
    bool is_ipaddr(const char* ip);
};

#endif /* _IpValidCheck_H */
