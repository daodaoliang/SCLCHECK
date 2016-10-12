/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: E9DFBC
 * Name: LDdoiCheck
 * Desc: LN0/LN、DOI、SDI、DAI对数据类型模板引用的有效性（实例化配置DOI/SDI/DAI的对象索引是否与数据类型模板定义的数据对象结构一致）
 * Ref: GDW1396
 */

#ifndef _LDdoiCheck_H
#define _LDdoiCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CLDdoiCheck : public CSingleInstance<CLDdoiCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLDdoiCheck();

public:
    virtual bool CheckInternal();

    static bool CheckLnTypeValid_InCacheMgr(LNCache& ln, DOMap& mapDo);
    static bool CheckDOTypeValid_InCacheMgr(DOICache& doi, DOMap& mapDo, DOTypeCache& doType);
    static void CheckDATypeValid_InCacheMgr(int offset, wstring sDaName, SdiCache& sdi);
    static bool CheckSDITypeValid_InCacheMgr(int offset, wstring sSdiName, SdiCache& sdi);

};

#endif /* _LDdoiCheck_H */
