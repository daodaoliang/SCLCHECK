/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 386A45
 * Name: SCDLNodeTypeCheck
 * Desc: LNodeType�����Լ��
 * Ref: ���ܱ��վSCD��鼼���淶
 */

#ifndef _SCDLNodeTypeCheck_H
#define _SCDLNodeTypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDLNodeTypeCheck : public CSingleInstance<CSCDLNodeTypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDLNodeTypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDLNodeTypeCheck_H */
