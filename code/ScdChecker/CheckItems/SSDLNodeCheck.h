/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: FBA7BB
 * Name: SSDLNodeCheck
 * Desc: LNodeԪ���Ƿ���Ϲ�����׼��
 * Ref: �����ܱ��վSSD��ģ�����淶����¼E.3
 */

#ifndef _SSDLNodeCheck_H
#define _SSDLNodeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDLNodeCheck : public CSingleInstance<CSSDLNodeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDLNodeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDLNodeCheck_H */
