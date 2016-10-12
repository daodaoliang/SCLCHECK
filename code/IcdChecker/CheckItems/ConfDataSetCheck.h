/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: AF455E
 * Name: ConfDataSetCheck
 * Desc: 数据集DataSet>实际个数是否小于或等于ConfDataset的max属性值
 * Ref: GDW1396
 */

#ifndef _ConfDataSetCheck_H
#define _ConfDataSetCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CConfDataSetCheck : public CSingleInstance<CConfDataSetCheck>, CNoCopy, public CCheckItemBase
{
public:
    CConfDataSetCheck();

public:
    virtual bool CheckInternal();

    static void CheckDataset_InCacheMgr(ICDCache& icd);
};

#endif /* _ConfDataSetCheck_H */
