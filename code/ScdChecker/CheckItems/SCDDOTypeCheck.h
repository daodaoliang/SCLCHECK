/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 296AE3
 * Name: SCDDOTypeCheck
 * Desc: DOType�����Լ��
 * Ref: ���ܱ��վSCD��鼼���淶
 */

#ifndef _SCDDOTypeCheck_H
#define _SCDDOTypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDDOTypeCheck : public CSingleInstance<CSCDDOTypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDDOTypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDDOTypeCheck_H */
