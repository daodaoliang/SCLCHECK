/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 83395E
 * Name: SmvAppidValidCheck
 * Desc: Communication��SMV��APPID��ֵ�Ƿ�Ϊ4λ16����ֵ���Ƿ���4000��7FFFȡֵ��Χ���Ҳ���Ϊ0
 * Ref: GDW1396
 */

#ifndef _SmvAppidValidCheck_H
#define _SmvAppidValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSmvAppidValidCheck : public CSingleInstance<CSmvAppidValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSmvAppidValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SmvAppidValidCheck_H */
