/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: C426C5
 * Name: CommunicationGSEorSVUniqueCheck
 * Desc: Communication中访问点通信配置是否唯一
 * Ref: GDW1396
 */

#ifndef _CommunicationGSEorSVUniqueCheck_H
#define _CommunicationGSEorSVUniqueCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CCommunicationGSEorSVUniqueCheck : public CSingleInstance<CCommunicationGSEorSVUniqueCheck>, CNoCopy, public CCheckItemBase
{
public:
    CCommunicationGSEorSVUniqueCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _CommunicationGSEorSVUniqueCheck_H */
