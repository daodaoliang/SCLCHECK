/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: A4401A
 * Name: SSDBayCheck_AttrNormative
 * Desc: һ���豸���BayԪ�ص�name��desc�����Ƿ���Ϲ淶��
 * Ref: �����ܱ��վSSD��ģ�����淶��6.4.3.a)
 */

#ifndef _SSDBayCheck_AttrNormative_H
#define _SSDBayCheck_AttrNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDBayCheck_AttrNormative : public CSingleInstance<CSSDBayCheck_AttrNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDBayCheck_AttrNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDBayCheck_AttrNormative_H */
