/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 6AD663
 * Name: SSDVoltageLevelCheck_Other
 * Desc: VoltageLevel�Ƿ�������Ԫ�أ�LNode�ȣ�
 * Ref: �����ܱ��վSSD��ģ�����淶��6.3.b��
 */

#ifndef _SSDVoltageLevelCheck_Other_H
#define _SSDVoltageLevelCheck_Other_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck_Other : public CSingleInstance<CSSDVoltageLevelCheck_Other>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck_Other();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_Other_H */
