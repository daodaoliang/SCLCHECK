/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 4BA204
 * Name: SCDEnumTypeCheck
 * Desc: EnumType�����Լ��
 * Ref: ���ܱ��վSCD��鼼���淶
 */

#ifndef _SCDEnumTypeCheck_H
#define _SCDEnumTypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDEnumTypeCheck : public CSingleInstance<CSCDEnumTypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDEnumTypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDEnumTypeCheck_H */
