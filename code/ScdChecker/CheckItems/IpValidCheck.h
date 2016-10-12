/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 7053C8
 * Name: IpValidCheck
 * Desc: IP��ַ�����������ÿһλ��0~255��Χ��
 * Ref: GDW1396
 */

#ifndef _IpValidCheck_H
#define _IpValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CIpValidCheck : public CSingleInstance<CIpValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CIpValidCheck();

public:
    virtual bool CheckInternal();

    int StrToIntDef(const char *str, int default_value);
    bool is_ipaddr(const char* ip);
};

#endif /* _IpValidCheck_H */
