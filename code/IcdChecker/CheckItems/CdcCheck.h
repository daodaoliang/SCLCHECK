/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 1F61CB
 * Name: CdcCheck
 * Desc: 公用数据类不允许自行扩充（检查是否存在扩展的CDC类型，STG为新版396自行扩充）
 * Ref: Q/GDW 1396-2012 第7.1.5节
 */

#ifndef _CdcCheck_H
#define _CdcCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CCdcCheck : public CSingleInstance<CCdcCheck>, CNoCopy, public CCheckItemBase
{
public:
    CCdcCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _CdcCheck_H */
