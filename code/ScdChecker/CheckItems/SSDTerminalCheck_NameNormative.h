/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 88181E
 * Name: SSDTerminalCheck_NameNormative
 * Desc: Terminal对象name属性是否按照Tn进行命名
 * Ref: 《智能变电站SSD建模技术规范》6.5.6
 */

#ifndef _SSDTerminalCheck_NameNormative_H
#define _SSDTerminalCheck_NameNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDTerminalCheck_NameNormative : public CSingleInstance<CSSDTerminalCheck_NameNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDTerminalCheck_NameNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDTerminalCheck_NameNormative_H */
