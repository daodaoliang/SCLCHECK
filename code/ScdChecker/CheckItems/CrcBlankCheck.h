/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: DECEFE
 * Name: CrcBlankCheck
 * Desc: SCD文件中是否存在IED过程层虚端子CRC32校验码和全站过程层虚端子CRC32校验码
 * Ref: GDW1396
 */

#ifndef _CrcBlankCheck_H
#define _CrcBlankCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CCrcBlankCheck : public CSingleInstance<CCrcBlankCheck>, CNoCopy, public CCheckItemBase
{
public:
    CCrcBlankCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _CrcBlankCheck_H */
