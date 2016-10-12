/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 83395E
 * Name: SmvAppidValidCheck
 * Desc: Communication下SMV的APPID的值是否为4位16进制值，是否在4000～7FFF取值范围内且不可为0
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
