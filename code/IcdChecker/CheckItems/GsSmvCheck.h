/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 467747
 * Name: GsSmvCheck
 * Desc: IED��Ӧ����GSEControl��SampledValueControl��
 * Ref: ����ʵ��
 */

#ifndef _GsSmvCheck_H
#define _GsSmvCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CGsSmvCheck : public CSingleInstance<CGsSmvCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGsSmvCheck();

public:
    virtual bool CheckInternal();

    static void CheckGseSmv_InCacheMgr(ICDCache& icd);
};

#endif /* _GsSmvCheck_H */
