/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 145768
 * Name: EnumtypeCheck
 * Desc: ����׼EnumType�����ͺ�ֵ�Ƿ���ȷ
 * Ref: GDW1396
 */

#ifndef _EnumtypeCheck_H
#define _EnumtypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CEnumtypeCheck : public CSingleInstance<CEnumtypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CEnumtypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _EnumtypeCheck_H */
