/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: CAFCE7
 * Name: IedCheck
 * Desc: IED名称应为“TEMPLATE”
 * Ref: Q/GDW 1396-2012 第7.1.1节
 */

#ifndef _IedCheck_H
#define _IedCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CIedCheck : public CSingleInstance<CIedCheck>, CNoCopy, public CCheckItemBase
{
public:
    CIedCheck();

public:
    virtual bool CheckInternal();

    static void CheckIedName_InCacheMgr(std::wstring sIedName, int offset);
};

#endif /* _IedCheck_H */
