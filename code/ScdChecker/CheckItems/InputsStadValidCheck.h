/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: E58327
 * Name: InputsStadValidCheck
 * Desc: ������ͳһ�����������ӵ��ź�û�����ӻ����Ӳ���ȷ
 * Ref: GDW1396
 */

#ifndef _InputsStadValidCheck_H
#define _InputsStadValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsStadValidCheck : public CSingleInstance<CInputsStadValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsStadValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsStadValidCheck_H */
