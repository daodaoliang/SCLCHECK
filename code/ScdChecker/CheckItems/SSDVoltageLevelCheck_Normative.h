/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 221E7A
 * Name: SSDVoltageLevelCheck_Normative
 * Desc: VoltageLevel��name��desc�����Ƿ���϶��ѹ��׼
 * Ref: �����ܱ��վSSD��ģ�����淶��6.3.b��
 */

#ifndef _SSDVoltageLevelCheck_Normative_H
#define _SSDVoltageLevelCheck_Normative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck_Normative : public CSingleInstance<CSSDVoltageLevelCheck_Normative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck_Normative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_Normative_H */
