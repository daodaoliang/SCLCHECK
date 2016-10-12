/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 7884C7
 * Name: GseAppidValidCheck
 * Desc: Communication��GSE��APPID��ֵ�Ƿ�Ϊ4λ16����ֵ���Ƿ���0000��3FFFȡֵ��Χ��
 * Ref: GDW1396
 */

#ifndef _GseAppidValidCheck_H
#define _GseAppidValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGseAppidValidCheck : public CSingleInstance<CGseAppidValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGseAppidValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GseAppidValidCheck_H */
