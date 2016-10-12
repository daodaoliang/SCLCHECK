/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: BD00E3
 * Name: SSDPowerTransformerCheck_Connection
 * Desc: PowerTransformer�Ƿ������Substation��VoltageLevel
 * Ref: һ��Ҫ��
 */

#ifndef _SSDPowerTransformerCheck_Connection_H
#define _SSDPowerTransformerCheck_Connection_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDPowerTransformerCheck_Connection : public CSingleInstance<CSSDPowerTransformerCheck_Connection>, CNoCopy, public CCheckItemBase
{
public:
    CSSDPowerTransformerCheck_Connection();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDPowerTransformerCheck_Connection_H */
