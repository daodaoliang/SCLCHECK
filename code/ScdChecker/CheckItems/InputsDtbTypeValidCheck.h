/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: F68BD2
 * Name: InputsDtbTypeValidCheck
 * Desc: SCD�ļ�����������ߵĶ������������շ�˫��bType�Ƿ�һ�£�
 * Ref: GDW1396
 */

#ifndef _InputsDtbTypeValidCheck_H
#define _InputsDtbTypeValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDtbTypeValidCheck : public CSingleInstance<CInputsDtbTypeValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDtbTypeValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDtbTypeValidCheck_H */
