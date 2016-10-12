/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: BB4024
 * Name: MDotypeCheck
 * Desc: ����׼DOType��ǿ��DAԪ���Ƿ����
 * Ref: Q/GDW 1396-2012 ��7.1.6��
 */

#ifndef _MDotypeCheck_H
#define _MDotypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../../ScdCheckCommon/ValidateCache.h"

class CMDotypeCheck : public CSingleInstance<CMDotypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CMDotypeCheck();

public:
    virtual bool CheckInternal();

private:
    // ����׼DOType��ǿ��DAԪ���Ƿ����
    void CheckMDOTypeDA(DOTypeCache& inst, DOTypeCache& temp);
};

#endif /* _MDotypeCheck_H */
