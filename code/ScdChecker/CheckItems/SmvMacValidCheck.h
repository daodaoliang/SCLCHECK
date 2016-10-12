/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: EA80EF
 * Name: SmvMacValidCheck
 * Desc: SMV��MAC��ַ��01-0C-CD-04-00-00~01-0C-CD-04-FF-FF��Χ��
 * Ref: GDW1396
 */

#ifndef _SmvMacValidCheck_H
#define _SmvMacValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSmvMacValidCheck : public CSingleInstance<CSmvMacValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSmvMacValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SmvMacValidCheck_H */
