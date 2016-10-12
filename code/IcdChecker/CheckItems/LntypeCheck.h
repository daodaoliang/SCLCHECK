/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 5BA640
 * Name: LntypeCheck
 * Desc: ��׼LNodeType��飨����׼LNodeType�ı�׼DO˳���Ƿ���ȷ������׼LNodeType�ı�׼DO�����������Ƿ���ȷ����׼LNodeType�Ƿ������չ��DO���ͣ�
 * Ref: GDW1396
 */

#ifndef _LntypeCheck_H
#define _LntypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

#include "..\..\ScdCheckCommon\ValidateCache.h"

class CLntypeCheck : public CSingleInstance<CLntypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLntypeCheck();

public:
    virtual bool CheckInternal();

public:

private:
    // ���DO��˳�������Ƿ����׼ģ����һ��
    void CheckDO(LNodeTypeCache& inst, LNodeTypeCache& temp);
    
};

#endif /* _LntypeCheck_H */
