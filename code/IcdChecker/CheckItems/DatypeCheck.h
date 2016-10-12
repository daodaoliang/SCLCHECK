/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: DE1D56
 * Name: DatypeCheck
 * Desc: ����������Q/GDW-1396��׼�ж���������������ͣ�DAType����Ӧ���׼�ж��������һ��(���DAType��BDAԪ��˳���Ƿ�һ�£����DAType�����������Ƿ���ȷ)
 * Ref: ����ʵ����SCD������У�����ռ���׼
 */

#ifndef _DatypeCheck_H
#define _DatypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../../ScdCheckCommon/ValidateCache.h"

class CDatypeCheck : public CSingleInstance<CDatypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDatypeCheck();

public:
    virtual bool CheckInternal();

private:
    void CheckDAType(DATypeCache& instDA, DATypeCache& tempDA);
};

#endif /* _DatypeCheck_H */
