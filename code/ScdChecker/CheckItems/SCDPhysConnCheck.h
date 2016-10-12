/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 0775FF
 * Name: SCDPhysConnCheck
 * Desc: PhysConn元素检测
 * Ref: GDW1396
 */

#ifndef _SCDPhysConnCheck_H
#define _SCDPhysConnCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDPhysConnCheck : public CSingleInstance<CSCDPhysConnCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDPhysConnCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDPhysConnCheck_H */
