/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: B73E6D
 * Name: MDatypeCheck
 * Desc: ��������ģ��ǿ���ż��DAType��ǿ��BDAԪ���Ƿ����Ϣ��⣨LNodeType��ǿ��DO�Ƿ���ȷ��
 * Ref: GDW1396
 */

#ifndef _MDatypeCheck_H
#define _MDatypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CMDatypeCheck : public CSingleInstance<CMDatypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CMDatypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _MDatypeCheck_H */
