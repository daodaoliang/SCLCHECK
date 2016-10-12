/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 7884C7
 * Name: GseAppidValidCheck
 * Desc: Communication下GSE的APPID的值是否为4位16进制值，是否在0000到3FFF取值范围内
 * Ref: GDW1396
 */

#ifndef _GseAppidValidCheck_H
#define _GseAppidValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGseAppidValidCheck : public CSingleInstance<CGseAppidValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGseAppidValidCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GseAppidValidCheck_H */
