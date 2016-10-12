/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: B179A8
 * Name: SSDVoltageLevelCheck_Unique
 * Desc: VoltageLevel�Ƿ�Ψһ
 * Ref: �����ܱ��վSSD��ģ�����淶��6.3.b)
 */

#ifndef _SSDVoltageLevelCheck_Unique_H
#define _SSDVoltageLevelCheck_Unique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck_Unique : public CSingleInstance<CSSDVoltageLevelCheck_Unique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck_Unique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_Unique_H */
