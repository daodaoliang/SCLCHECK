/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 90F38A
 * Name: SSDLNodeCheck_InBay
 * Desc: LNode�Ƿ��ڼ����
 * Ref: �����ܱ��վSSD��ģ�����淶��5.4
 */

#ifndef _SSDLNodeCheck_InBay_H
#define _SSDLNodeCheck_InBay_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDLNodeCheck_InBay : public CSingleInstance<CSSDLNodeCheck_InBay>, CNoCopy, public CCheckItemBase
{
public:
    CSSDLNodeCheck_InBay();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDLNodeCheck_InBay_H */
