/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: C426C5
 * Name: CommunicationGSEorSVUniqueCheck
 * Desc: Communication�з��ʵ�ͨ�������Ƿ�Ψһ
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
