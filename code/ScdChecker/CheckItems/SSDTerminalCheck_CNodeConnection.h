/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 7F77BD
 * Name: SSDTerminalCheck_CNodeConnection
 * Desc: Terminal�Ƿ��ConnectivityNode����
 * Ref: �����ܱ��վSSD��ģ�����淶��5.3.c��
 */

#ifndef _SSDTerminalCheck_CNodeConnection_H
#define _SSDTerminalCheck_CNodeConnection_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDTerminalCheck_CNodeConnection : public CSingleInstance<CSSDTerminalCheck_CNodeConnection>, CNoCopy, public CCheckItemBase
{
public:
    CSSDTerminalCheck_CNodeConnection();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDTerminalCheck_CNodeConnection_H */
