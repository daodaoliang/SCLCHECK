/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 8F38E4
 * Name: SSDVoltageLevelCheck
 * Desc: VoltageLevelԪ���Ƿ���Ϲ淶��
 * Ref: �����ܱ��վSSD��ģ�����淶����¼E.2
 */

#ifndef _SSDVoltageLevelCheck_H
#define _SSDVoltageLevelCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck : public CSingleInstance<CSSDVoltageLevelCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_H */
