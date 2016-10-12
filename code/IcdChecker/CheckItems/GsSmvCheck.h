/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 467747
 * Name: GsSmvCheck
 * Desc: IED中应包含GSEControl和SampledValueControl。
 * Ref: 工程实践
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
