/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 2DE06E
 * Name: SSDLNodeCheck_Valid
 * Desc: LNode�����Ƿ���Ч
 * Ref: һ��淶
 */

#ifndef _SSDLNodeCheck_Valid_H
#define _SSDLNodeCheck_Valid_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDLNodeCheck_Valid : public CSingleInstance<CSSDLNodeCheck_Valid>, CNoCopy, public CCheckItemBase
{
public:
    CSSDLNodeCheck_Valid();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDLNodeCheck_Valid_H */
