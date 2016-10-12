/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 353D3C
 * Name: SSDVoltageLevelCheck_Sort
 * Desc: 电压等级是否按照从高到低的顺序进行排序
 * Ref: 《智能变电站SSD建模技术规范》6.3.a）
 */

#ifndef _SSDVoltageLevelCheck_Sort_H
#define _SSDVoltageLevelCheck_Sort_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck_Sort : public CSingleInstance<CSSDVoltageLevelCheck_Sort>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck_Sort();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_Sort_H */
