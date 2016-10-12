/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 6A2E19
 * Name: LdValidCheck
 * Desc: LDevice������Ӧ����Q/GDW-1396��׼�ж��������ԭ��
 * Ref: Q/GDW 1396-2012 ��7.1.3��
 */

#ifndef _LdValidCheck_H
#define _LdValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CLdValidCheck : public CSingleInstance<CLdValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLdValidCheck();

public:
    virtual bool CheckInternal();

    void CheckName_InCacheMgr(std::wstring sName, int offset);

};

#endif /* _LdValidCheck_H */
