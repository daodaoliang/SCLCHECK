/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 83395E
 * Name: DataSetCheck
 * Desc: 数据集的命名应符合Q/GDW-1396标准中定义的命名原则。
 * Ref: Q/GDW 1396-2012 第8.3.1节
 */

#ifndef _DataSetCheck_H
#define _DataSetCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CDataSetCheck : public CSingleInstance<CDataSetCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDataSetCheck();

public:
    virtual bool CheckInternal();

    static void CheckName_InCacheMgr(DatasetCache& dateset, int offset);
};

#endif /* _DataSetCheck_H */
