/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 5E2045
 * Name: ConnectedAPValidCheck
 * Desc: Communication下ConnectedAP的iedName、apName以及GSE\SMV的cbName、ldInst的属性值是否在IED中存在相应信息
 * Ref: GDW1396
 */

#ifndef _ConnectedAPValidCheck_H
#define _ConnectedAPValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CConnectedAPValidCheck : public CSingleInstance<CConnectedAPValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CConnectedAPValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _ConnectedAPValidCheck_H */
