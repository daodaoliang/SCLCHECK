/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 353D3C
 * Name: SSDVoltageLevelCheck_Sort
 * Desc: ��ѹ�ȼ��Ƿ��մӸߵ��͵�˳���������
 * Ref: �����ܱ��վSSD��ģ�����淶��6.3.a��
 */

#ifndef _SSDVoltageLevelCheck_Sort_H
#define _SSDVoltageLevelCheck_Sort_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck_Sort : public CSingleInstance<CSSDVoltageLevelCheck_Sort>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck_Sort();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_Sort_H */
