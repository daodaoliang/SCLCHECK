/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: F49CBC
 * Name: SSDPowerTransformerCheck_AttrNormative
 * Desc: 变压器设备name、desc属性是否存在且符合国网标准（标准见“附件” “表2 一次设备命名表”）
 * Ref: 《智能变电站SSD建模技术规范》6.5.1.a）
 */

#ifndef _SSDPowerTransformerCheck_AttrNormative_H
#define _SSDPowerTransformerCheck_AttrNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDPowerTransformerCheck_AttrNormative : public CSingleInstance<CSSDPowerTransformerCheck_AttrNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDPowerTransformerCheck_AttrNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDPowerTransformerCheck_AttrNormative_H */
