/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 3C04EC
 * Name: SSDBayCheck_dTypeNormative
 * Desc: һ���豸���BayԪ�ص���չ����dType�Ƿ���Ϲ淶
 * Ref: �����ܱ��վSSD��ģ�����淶��6.4.3.b)
 */

#ifndef _SSDBayCheck_dTypeNormative_H
#define _SSDBayCheck_dTypeNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDBayCheck_dTypeNormative : public CSingleInstance<CSSDBayCheck_dTypeNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDBayCheck_dTypeNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDBayCheck_dTypeNormative_H */
