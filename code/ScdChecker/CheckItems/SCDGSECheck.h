/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 2391A5
 * Name: SCDGSECheck
 * Desc: GSE元素检测
 * Ref: GDW1396
 */

#ifndef _SCDGSECheck_H
#define _SCDGSECheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDGSECheck : public CSingleInstance<CSCDGSECheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDGSECheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDGSECheck_H */
