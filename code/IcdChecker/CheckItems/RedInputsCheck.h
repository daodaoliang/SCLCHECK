/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 51A518
 * Name: RedInputsCheck
 * Desc: IED�в�Ӧ����Inputs�������GOOSE/SV���ߵ���Ϣ��
 * Ref: ����ʵ��
 */

#ifndef _RedInputsCheck_H
#define _RedInputsCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CRedInputsCheck : public CSingleInstance<CRedInputsCheck>, CNoCopy, public CCheckItemBase
{
public:
    CRedInputsCheck();

public:
    virtual bool CheckInternal();

    static void CheckInputs_InCacheMgr(int offset);
};

#endif /* _RedInputsCheck_H */
