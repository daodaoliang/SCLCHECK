/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 165A0B
 * Name: RedDatypeCheck
 * Desc: 检查是否存在扩展DA，且扩展DA应符合命名规范（制造厂商前缀_装置型号_版本号_DA类名(_其它后缀)）
 * Ref: 工程实践（SCD检测已有）或参照检测标准
 */

#ifndef _RedDatypeCheck_H
#define _RedDatypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CRedDatypeCheck : public CSingleInstance<CRedDatypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CRedDatypeCheck();

public:
    virtual bool CheckInternal();

};

#endif /* _RedDatypeCheck_H */
