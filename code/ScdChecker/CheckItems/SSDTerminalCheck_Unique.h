/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: A27269
 * Name: SSDTerminalCheck_Unique
 * Desc: Terminal����װ�����Ƿ�Ψһ
 * Ref: һ��Ҫ��
 */

#ifndef _SSDTerminalCheck_Unique_H
#define _SSDTerminalCheck_Unique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDTerminalCheck_Unique : public CSingleInstance<CSSDTerminalCheck_Unique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDTerminalCheck_Unique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDTerminalCheck_Unique_H */
