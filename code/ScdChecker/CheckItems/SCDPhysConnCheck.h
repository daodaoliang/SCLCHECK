/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 0775FF
 * Name: SCDPhysConnCheck
 * Desc: PhysConnԪ�ؼ��
 * Ref: GDW1396
 */

#ifndef _SCDPhysConnCheck_H
#define _SCDPhysConnCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDPhysConnCheck : public CSingleInstance<CSCDPhysConnCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDPhysConnCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDPhysConnCheck_H */
