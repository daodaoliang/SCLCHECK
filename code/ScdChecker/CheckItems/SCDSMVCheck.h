/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: D0620B
 * Name: SCDSMVCheck
 * Desc: SMV元素检测
 * Ref: GDW1396
 */

#ifndef _SCDSMVCheck_H
#define _SCDSMVCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDSMVCheck : public CSingleInstance<CSCDSMVCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDSMVCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDSMVCheck_H */
