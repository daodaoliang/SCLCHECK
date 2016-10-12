/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: A63491
 * Name: RedDotypeCheck
 * Desc: ����Ƿ������չ��DO����չʱӦ���������淶�����쳧��ǰ׺_װ���ͺ�_�汾��_DO����(_������׺)��
 * Ref: Q/GDW 1396-2012 ��7.1.6��
 */

#ifndef _RedDotypeCheck_H
#define _RedDotypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../../ScdCheckCommon/ValidateCache.h"

class CRedDotypeCheck : public CSingleInstance<CRedDotypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CRedDotypeCheck();

public:
    virtual bool CheckInternal();

private:
    void CheckDOType(DOTypeCache& instDOType);
};

#endif /* _RedDotypeCheck_H */
