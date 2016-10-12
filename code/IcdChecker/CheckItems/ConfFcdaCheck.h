/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: B3824A
 * Name: ConfFcdaCheck
 * Desc: 数据集配置的FCDA实际个数是否小于或等于ConfDataset的maxAttributes属性值（单个数据集中包含的FCDA个数不应为0也不应超过256，避免信号过多无法上送）
 * Ref: GDW1396
 */

#ifndef _ConfFcdaCheck_H
#define _ConfFcdaCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CConfFcdaCheck : public CSingleInstance<CConfFcdaCheck>, CNoCopy, public CCheckItemBase
{
public:
    CConfFcdaCheck();

public:
    virtual bool CheckInternal();

    static void CheckFcdaCount_InCacheMgr(DatasetCache& dataset, int maxFcda);
};

#endif /* _ConfFcdaCheck_H */
