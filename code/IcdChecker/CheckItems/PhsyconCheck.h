/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: ED195B
 * Name: PhsyconCheck
 * Desc: Communication��PhysConn���õ�����˿ڲ�ͷ���͡��˿ںš��ӿ����͵��Ƿ���Ϲ淶�е�����Ҫ��
 * Ref: GDW1396
 */

#ifndef _PhsyconCheck_H
#define _PhsyconCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CPhsyconCheck : public CSingleInstance<CPhsyconCheck>, CNoCopy, public CCheckItemBase
{
public:
    CPhsyconCheck();

public:
    virtual bool CheckInternal();

    void CheckPortType_InCacheMgr(std::wstring sType, std::wstring sValue, int offset);
};

#endif /* _PhsyconCheck_H */
