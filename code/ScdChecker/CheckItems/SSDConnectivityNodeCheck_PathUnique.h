/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: A897DD
 * Name: SSDConnectivityNodeCheck_PathUnique
 * Desc: ConnectivityNode��pathName�Ƿ�ȫվΨһ
 * Ref: �����ܱ��վSCD��鼼���淶��7.2.3
 */

#ifndef _SSDConnectivityNodeCheck_PathUnique_H
#define _SSDConnectivityNodeCheck_PathUnique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConnectivityNodeCheck_PathUnique : public CSingleInstance<CSSDConnectivityNodeCheck_PathUnique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConnectivityNodeCheck_PathUnique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConnectivityNodeCheck_PathUnique_H */
