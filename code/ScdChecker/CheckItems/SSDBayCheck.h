/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 5EEDF0
 * Name: SSDBayCheck
 * Desc: BayԪ���Ƿ���Ϲ淶��
 * Ref: �����ܱ��վSSD��ģ�����淶����¼E.3
 */

#ifndef _SSDBayCheck_H
#define _SSDBayCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDBayCheck : public CSingleInstance<CSSDBayCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSSDBayCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDBayCheck_H */
