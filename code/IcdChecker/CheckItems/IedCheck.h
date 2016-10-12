/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: CAFCE7
 * Name: IedCheck
 * Desc: IED����ӦΪ��TEMPLATE��
 * Ref: Q/GDW 1396-2012 ��7.1.1��
 */

#ifndef _IedCheck_H
#define _IedCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CIedCheck : public CSingleInstance<CIedCheck>, CNoCopy, public CCheckItemBase
{
public:
    CIedCheck();

public:
    virtual bool CheckInternal();

    static void CheckIedName_InCacheMgr(std::wstring sIedName, int offset);
};

#endif /* _IedCheck_H */
