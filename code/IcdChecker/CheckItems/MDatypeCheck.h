/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: B73E6D
 * Name: MDatypeCheck
 * Desc: 数据类型模板强制信检查DAType的强制BDA元素是否存在息检测（LNodeType的强制DO是否正确）
 * Ref: GDW1396
 */

#ifndef _MDatypeCheck_H
#define _MDatypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CMDatypeCheck : public CSingleInstance<CMDatypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CMDatypeCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _MDatypeCheck_H */
