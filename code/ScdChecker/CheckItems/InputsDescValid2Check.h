/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 189315
 * Name: InputsDescValid2Check
 * Desc: ������λ���Ӵ�����բ����ʧ����λ���Ӵ���
 * Ref: GDW1396
 */

#ifndef _InputsDescValid2Check_H
#define _InputsDescValid2Check_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDescValid2Check : public CSingleInstance<CInputsDescValid2Check>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDescValid2Check();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDescValid2Check_H */
