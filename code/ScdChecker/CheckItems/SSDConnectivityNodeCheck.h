/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 7BDF46
 * Name: SSDConnectivityNodeCheck
 * Desc: ConnectivityNode�Ƿ���Ϲ�����׼��
 * Ref: �����ܱ��վSSD��ģ�����淶����¼E.6
 */

#ifndef _SSDConnectivityNodeCheck_H
#define _SSDConnectivityNodeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConnectivityNodeCheck : public CSingleInstance<CSSDConnectivityNodeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConnectivityNodeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConnectivityNodeCheck_H */
