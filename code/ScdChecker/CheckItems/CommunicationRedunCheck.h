/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 3D2A1B
 * Name: CommunicationRedunCheck
 * Desc: GOOSE��SVδ�����ģ���ͨ�Ų���δ����
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
