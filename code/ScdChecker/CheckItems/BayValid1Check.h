/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 06857F
 * Name: BayValid1Check
 * Desc: ��������ء��ϲ���Ԫ�������ն˼�����Ӵ���
 * Ref: GDW1396
 */

#ifndef _BayValid1Check_H
#define _BayValid1Check_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CBayValid1Check : public CSingleInstance<CBayValid1Check>, CNoCopy, public CCheckItemBase
{
public:
    CBayValid1Check();

public:
    virtual bool CheckInternal();
};

#endif /* _BayValid1Check_H */
