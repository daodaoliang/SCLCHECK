/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 165A0B
 * Name: RedDatypeCheck
 * Desc: ����Ƿ������չDA������չDAӦ���������淶�����쳧��ǰ׺_װ���ͺ�_�汾��_DA����(_������׺)��
 * Ref: ����ʵ����SCD������У�����ռ���׼
 */

#ifndef _RedDatypeCheck_H
#define _RedDatypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CRedDatypeCheck : public CSingleInstance<CRedDatypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CRedDatypeCheck();

public:
    virtual bool CheckInternal();

};

#endif /* _RedDatypeCheck_H */
