/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: DECEFE
 * Name: CrcBlankCheck
 * Desc: SCD�ļ����Ƿ����IED���̲������CRC32У�����ȫվ���̲������CRC32У����
 * Ref: GDW1396
 */

#ifndef _CrcBlankCheck_H
#define _CrcBlankCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CCrcBlankCheck : public CSingleInstance<CCrcBlankCheck>, CNoCopy, public CCheckItemBase
{
public:
    CCrcBlankCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _CrcBlankCheck_H */
