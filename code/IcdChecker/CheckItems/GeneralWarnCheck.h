/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 5E04BF
 * Name: GeneralWarnCheck
 * Desc: ����������
 * Ref: GDW1396
 */

#ifndef _GeneralWarnCheck_H
#define _GeneralWarnCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGeneralWarnCheck : public CSingleInstance<CGeneralWarnCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGeneralWarnCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GeneralWarnCheck_H */
