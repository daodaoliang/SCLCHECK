/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 50B26C
 * Name: SSDTerminalCheck
 * Desc: �豸�˵�Terminal�����Ƿ���ϱ�׼
 * Ref: �����ܱ��վSSD��ģ�����淶����¼E.4
 */

#ifndef _SSDTerminalCheck_H
#define _SSDTerminalCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDTerminalCheck : public CSingleInstance<CSSDTerminalCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDTerminalCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDTerminalCheck_H */
