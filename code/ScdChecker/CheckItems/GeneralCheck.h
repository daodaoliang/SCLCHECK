/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 5E04BF
 * Name: GeneralCheck
 * Desc: ����������
 * Ref: GDW1396
 */

#ifndef _GeneralCheck_H
#define _GeneralCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGeneralCheck : public CSingleInstance<CGeneralCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGeneralCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GeneralCheck_H */
