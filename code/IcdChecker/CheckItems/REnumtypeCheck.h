/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 56AB89
 * Name: REnumtypeCheck
 * Desc: ����Ƿ������չ��EnumType
 * Ref: GDW1396
 */

#ifndef _REnumtypeCheck_H
#define _REnumtypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CREnumtypeCheck : public CSingleInstance<CREnumtypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CREnumtypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _REnumtypeCheck_H */
