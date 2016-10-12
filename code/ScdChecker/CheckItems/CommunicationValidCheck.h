/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 82EDB9
 * Name: CommunicationValidCheck
 * Desc: SCD文件中存在订阅关系的虚端子，对应发送控制块的通信参数配置是否存在；
 * Ref: GDW1396
 */

#ifndef _CommunicationValidCheck_H
#define _CommunicationValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CCommunicationValidCheck : public CSingleInstance<CCommunicationValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CCommunicationValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _CommunicationValidCheck_H */
