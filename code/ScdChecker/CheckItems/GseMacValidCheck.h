/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 58E4DA
 * Name: GseMacValidCheck
 * Desc: GSE��MAC��ַ��01-0C-CD-01-00-00~01-0C-CD-01-FF-FF��Χ��
 * Ref: GDW1396
 */

#ifndef _GseMacValidCheck_H
#define _GseMacValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGseMacValidCheck : public CSingleInstance<CGseMacValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGseMacValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GseMacValidCheck_H */
