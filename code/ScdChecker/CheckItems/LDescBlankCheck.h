/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 71A77B
 * Name: LDescBlankCheck
 * Desc: LD��LNʵ���ġ�desc������ֵ�Ƿ�ǿ�
 * Ref: GDW1396
 */

#ifndef _LDescBlankCheck_H
#define _LDescBlankCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CLDescBlankCheck : public CSingleInstance<CLDescBlankCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLDescBlankCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _LDescBlankCheck_H */
