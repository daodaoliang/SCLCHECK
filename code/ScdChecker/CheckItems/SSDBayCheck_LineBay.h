/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 3AFF97
 * Name: SSDBayCheck_LineBay
 * Desc: ��·����Ƿ����������豸
 * Ref: �����ܱ��վSSD��ģ�����淶��6.4.3.e)
 */

#ifndef _SSDBayCheck_LineBay_H
#define _SSDBayCheck_LineBay_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDBayCheck_LineBay : public CSingleInstance<CSSDBayCheck_LineBay>, CNoCopy, public CCheckItemBase
{
public:
    CSSDBayCheck_LineBay();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDBayCheck_LineBay_H */
