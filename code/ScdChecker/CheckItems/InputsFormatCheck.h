/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 307BCA
 * Name: InputsFormatCheck
 * Desc: GOOSE ��SV���õ�ExtRef�ڲ�intAddr��ֵ�Ƿ���Ϲ淶�е�������ʽҪ�󣬹淶��ʽӦΪ��LD/LN.DO.DA��(Q/GDW 396-2009)��n-A:LD/LN.DO.DA����Q/GDW 396-2012����
 * Ref: GDW1396
 */

#ifndef _InputsFormatCheck_H
#define _InputsFormatCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsFormatCheck : public CSingleInstance<CInputsFormatCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsFormatCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsFormatCheck_H */
