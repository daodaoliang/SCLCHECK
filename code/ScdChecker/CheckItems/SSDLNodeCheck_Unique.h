/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 8A3C65
 * Name: SSDLNodeCheck_Unique
 * Desc: LNode�Ƿ�ȫվΨһ
 * Ref: һ��淶
 */

#ifndef _SSDLNodeCheck_Unique_H
#define _SSDLNodeCheck_Unique_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDLNodeCheck_Unique : public CSingleInstance<CSSDLNodeCheck_Unique>, CNoCopy, public CCheckItemBase
{
public:
    CSSDLNodeCheck_Unique();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDLNodeCheck_Unique_H */
