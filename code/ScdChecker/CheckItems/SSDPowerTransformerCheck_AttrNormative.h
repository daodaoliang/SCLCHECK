/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: F49CBC
 * Name: SSDPowerTransformerCheck_AttrNormative
 * Desc: ��ѹ���豸name��desc�����Ƿ�����ҷ��Ϲ�����׼����׼���������� ����2 һ���豸��������
 * Ref: �����ܱ��վSSD��ģ�����淶��6.5.1.a��
 */

#ifndef _SSDPowerTransformerCheck_AttrNormative_H
#define _SSDPowerTransformerCheck_AttrNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDPowerTransformerCheck_AttrNormative : public CSingleInstance<CSSDPowerTransformerCheck_AttrNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDPowerTransformerCheck_AttrNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDPowerTransformerCheck_AttrNormative_H */
