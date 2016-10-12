/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 8FE372
 * Name: DaTempValidCheck
 * Desc: ʵ��������DOI/SDI/DAI�Ķ��������Ƿ�����������ģ�嶨������ݶ���ṹһ��
 * Ref: GDW1396
 */

#ifndef _DaTempValidCheck_H
#define _DaTempValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CDaTempValidCheck : public CSingleInstance<CDaTempValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDaTempValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _DaTempValidCheck_H */
