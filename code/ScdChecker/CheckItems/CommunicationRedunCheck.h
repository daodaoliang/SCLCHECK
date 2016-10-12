/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 3D2A1B
 * Name: CommunicationRedunCheck
 * Desc: GOOSE、SV未被订阅，且通信参数未配置
 * Ref: GDW1396
 */

#ifndef _CommunicationRedunCheck_H
#define _CommunicationRedunCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CCommunicationRedunCheck : public CSingleInstance<CCommunicationRedunCheck>, CNoCopy, public CCheckItemBase
{
public:
    CCommunicationRedunCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _CommunicationRedunCheck_H */
