/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: D25578
 * Name: DoiDescBlankCheck
 * Desc: ʵ����DOIԪ�صġ�desc������ֵ�롰dU�� ����ֵ�Ƿ�ǿ�������һ�£�
 * Ref: GDW1396
 */

#ifndef _DoiDescBlankCheck_H
#define _DoiDescBlankCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CDoiDescBlankCheck : public CSingleInstance<CDoiDescBlankCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDoiDescBlankCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _DoiDescBlankCheck_H */
