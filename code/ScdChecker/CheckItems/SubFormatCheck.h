/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: D400F3
 * Name: SubFormatCheck
 * Desc: Communication��SubNetwork�����������Ƿ���Ϲ淶�е�����Ҫ��
 * Ref: GDW1396
 */

#ifndef _SubFormatCheck_H
#define _SubFormatCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSubFormatCheck : public CSingleInstance<CSubFormatCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSubFormatCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SubFormatCheck_H */
