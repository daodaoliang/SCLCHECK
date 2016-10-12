/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 20544B
 * Name: MLntypeCheck
 * Desc: ����׼LNodeType��ǿ�ƣ�������ǿ�ƣ�DO�Ƿ����
 * Ref: Q/GDW 1396-2012 ��7.1.7��
 */

#ifndef _MLntypeCheck_H
#define _MLntypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

#include "..\..\ScdCheckCommon\ValidateCache.h"

class CMLntypeCheck : public CSingleInstance<CMLntypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CMLntypeCheck();

public:
    virtual bool CheckInternal();

private:
    // ����׼LNodeType��ǿ�ƣ�������ǿ�ƣ�DO�Ƿ����
    void CheckMLNodeTypeDo(LNodeTypeCache& inst, LNodeTypeCache& temp);

};

#endif /* _MLntypeCheck_H */
