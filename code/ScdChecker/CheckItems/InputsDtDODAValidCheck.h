/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 795860
 * Name: InputsDtDODAValidCheck
 * Desc: SCD�ļ�����������ߵĶ������������շ�˫���Ƿ�ΪDO/DA��DA/DO��
 * Ref: GDW1396
 */

#ifndef _InputsDtDODAValidCheck_H
#define _InputsDtDODAValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDtDODAValidCheck : public CSingleInstance<CInputsDtDODAValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDtDODAValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDtDODAValidCheck_H */
