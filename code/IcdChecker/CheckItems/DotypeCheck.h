/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 3E1AE0
 * Name: DotypeCheck
 * Desc: ����������Q/GDW-1396��׼�ж�������ݶ������ͣ�DOType����Ӧ���׼�ж��������һ�¡�(���DOType�ı�׼DAԪ�ء�SDO��Ԫ��˳���Ƿ�һ�£����DO Type�����������Ƿ���ȷ)
 * Ref: GDW1396
 */

#ifndef _DotypeCheck_H
#define _DotypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../../ScdCheckCommon/ValidateCache.h"

class CDotypeCheck : public CSingleInstance<CDotypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDotypeCheck();

public:
    virtual bool CheckInternal();

private:
    void CheckDOType(DOTypeCache& instDOType);
};

#endif /* _DotypeCheck_H */
