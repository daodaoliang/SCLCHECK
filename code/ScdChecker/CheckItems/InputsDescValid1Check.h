/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 5A551E
 * Name: InputsDescValid1Check
 * Desc: �������֣���բ�ź���������ʧ�飬����ʧ���ź�������բ
 * Ref: GDW1396
 */

#ifndef _InputsDescValid1Check_H
#define _InputsDescValid1Check_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDescValid1Check : public CSingleInstance<CInputsDescValid1Check>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDescValid1Check();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDescValid1Check_H */
