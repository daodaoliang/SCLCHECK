/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 39B6E1
 * Name: InputsValidCheck
 * Desc: SCD�ļ�����������ߵĶ��������Ƿ���Ч��
 * Ref: GDW1396
 */

#ifndef _InputsValidCheck_H
#define _InputsValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsValidCheck : public CSingleInstance<CInputsValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsValidCheck_H */
