/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: EFA2C2
 * Name: SCDSubNetWorkCheck
 * Desc: SubnetWork元素检测
 * Ref: GDW1396
 */

#ifndef _SCDSubNetWorkCheck_H
#define _SCDSubNetWorkCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDSubNetWorkCheck : public CSingleInstance<CSCDSubNetWorkCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDSubNetWorkCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDSubNetWorkCheck_H */
