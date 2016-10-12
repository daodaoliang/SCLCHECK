/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: EA80EF
 * Name: SmvMacValidCheck
 * Desc: SMV中MAC地址在01-0C-CD-04-00-00~01-0C-CD-04-FF-FF范围内
 * Ref: GDW1396
 */

#ifndef _SmvMacValidCheck_H
#define _SmvMacValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSmvMacValidCheck : public CSingleInstance<CSmvMacValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSmvMacValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SmvMacValidCheck_H */
