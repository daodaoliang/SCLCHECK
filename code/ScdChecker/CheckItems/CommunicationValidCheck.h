/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 82EDB9
 * Name: CommunicationValidCheck
 * Desc: SCD�ļ��д��ڶ��Ĺ�ϵ������ӣ���Ӧ���Ϳ��ƿ��ͨ�Ų��������Ƿ���ڣ�
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
