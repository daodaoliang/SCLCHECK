/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 6D6D0B
 * Name: SSDConnectivityNodeCheck_NameNormative
 * Desc: ConnectivityNode��name�����Ƿ���Cn��������
 * Ref: �����ܱ��վSSD��ģ�����淶��6.5.5.a)
 */

#ifndef _SSDConnectivityNodeCheck_NameNormative_H
#define _SSDConnectivityNodeCheck_NameNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDConnectivityNodeCheck_NameNormative : public CSingleInstance<CSSDConnectivityNodeCheck_NameNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDConnectivityNodeCheck_NameNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDConnectivityNodeCheck_NameNormative_H */
