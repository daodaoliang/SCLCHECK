/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: B861B0
 * Name: GeneralErrorCheck
 * Desc: ����������
 * Ref: GDW1396
 */

#ifndef _GeneralErrorCheck_H
#define _GeneralErrorCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGeneralErrorCheck : public CSingleInstance<CGeneralErrorCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGeneralErrorCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GeneralErrorCheck_H */
