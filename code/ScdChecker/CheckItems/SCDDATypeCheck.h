/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 71D964
 * Name: SCDDATypeCheck
 * Desc: DAType�����Լ��
 * Ref: ���ܱ��վSCD��鼼���淶
 */

#ifndef _SCDDATypeCheck_H
#define _SCDDATypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDDATypeCheck : public CSingleInstance<CSCDDATypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDDATypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDDATypeCheck_H */
