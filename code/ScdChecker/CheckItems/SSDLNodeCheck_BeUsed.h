/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: D6F36C
 * Name: SSDLNodeCheck_BeUsed
 * Desc: LNode�Ƿ�ֻ������һ��
 * Ref: һ��淶
 */

#ifndef _SSDLNodeCheck_BeUsed_H
#define _SSDLNodeCheck_BeUsed_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDLNodeCheck_BeUsed : public CSingleInstance<CSSDLNodeCheck_BeUsed>, CNoCopy, public CCheckItemBase
{
public:
    CSSDLNodeCheck_BeUsed();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDLNodeCheck_BeUsed_H */
