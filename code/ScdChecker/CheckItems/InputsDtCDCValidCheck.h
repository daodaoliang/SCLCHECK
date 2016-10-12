/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 7652D1
 * Name: InputsDtCDCValidCheck
 * Desc: SCD�ļ�����������ߵĶ������������շ�˫��CDC�Ƿ�һ�£�
 * Ref: GDW1396
 */

#ifndef _InputsDtCDCValidCheck_H
#define _InputsDtCDCValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsDtCDCValidCheck : public CSingleInstance<CInputsDtCDCValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsDtCDCValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsDtCDCValidCheck_H */
