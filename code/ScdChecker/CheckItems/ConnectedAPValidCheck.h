/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 5E2045
 * Name: ConnectedAPValidCheck
 * Desc: Communication��ConnectedAP��iedName��apName�Լ�GSE\SMV��cbName��ldInst������ֵ�Ƿ���IED�д�����Ӧ��Ϣ
 * Ref: GDW1396
 */

#ifndef _ConnectedAPValidCheck_H
#define _ConnectedAPValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CConnectedAPValidCheck : public CSingleInstance<CConnectedAPValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CConnectedAPValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _ConnectedAPValidCheck_H */
