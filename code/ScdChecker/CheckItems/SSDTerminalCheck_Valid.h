/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 7BCC9B
 * Name: SSDTerminalCheck_Valid
 * Desc: Terminal��SubstationName��VoltageLevelName��bayName��cNodeName�����Ƿ���Ч
 * Ref: һ��Ҫ��
 */

#ifndef _SSDTerminalCheck_Valid_H
#define _SSDTerminalCheck_Valid_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDTerminalCheck_Valid : public CSingleInstance<CSSDTerminalCheck_Valid>, CNoCopy, public CCheckItemBase
{
public:
    CSSDTerminalCheck_Valid();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDTerminalCheck_Valid_H */
