/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: A63491
 * Name: FcdaValidCheck
 * Desc: ���ݼ����õ�FCDA�ǿյ�����ֵ�Ƿ�ָ��ģ�����Ѵ��ڵ����ݶ������������
 * Ref: GDW1396
 */

#ifndef _FcdaValidCheck_H
#define _FcdaValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CFcdaValidCheck : public CSingleInstance<CFcdaValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CFcdaValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _FcdaValidCheck_H */
