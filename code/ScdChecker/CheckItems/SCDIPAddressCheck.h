/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 25B22A
 * Name: SCDIPAddressCheck
 * Desc: IP����AddressԪ�ؼ��
 * Ref: GDW1396
 */

#ifndef _SCDIPAddressCheck_H
#define _SCDIPAddressCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDIPAddressCheck : public CSingleInstance<CSCDIPAddressCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDIPAddressCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDIPAddressCheck_H */
