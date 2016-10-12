/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: EA80EF
 * Name: DataSetTypeValidCheck
 * Desc: GOOSE数据集应配置到DA。其它配置（包括SV数据集）都到DO
 * Ref: Q/GDW 1396-2012 第10.1.1，11.1.1节
 */

#ifndef _DataSetTypeValidCheck_H
#define _DataSetTypeValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"
class CDataSetTypeValidCheck : public CSingleInstance<CDataSetTypeValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDataSetTypeValidCheck();

public:
    virtual bool CheckInternal();

    void CheckValid_InCacheMgr(std::wstring sCBType, FcdaCache& fcda);
};

#endif /* _DataSetTypeValidCheck_H */
