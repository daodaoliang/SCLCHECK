/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: A897DD
 * Name: SSDConnectivityNodeCheck_UnUsed
 * Desc: ConnectivityNodeδ��Terminal����
 * Ref: �����ܱ��վSCD��鼼���淶��7.2.3
 */

#ifndef _SSDConnectivityNodeCheck_UnUsed_H
#define _SSDConnectivityNodeCheck_UnUsed_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConnectivityNodeCheck_UnUsed : public CSingleInstance<CSSDConnectivityNodeCheck_UnUsed>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConnectivityNodeCheck_UnUsed();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConnectivityNodeCheck_UnUsed_H */
