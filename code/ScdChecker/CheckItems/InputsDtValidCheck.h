/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 795860
 * Name: InputsDtValidCheck
 * Desc: SCD�ļ�����������ߵĶ������������շ�˫���Ƿ�һ�»���ݣ�
 * Ref: GDW1396
 */

#ifndef _InputsDtValidCheck_H
#define _InputsDtValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDtValidCheck : public CSingleInstance<CInputsDtValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDtValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDtValidCheck_H */
