/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 5A05FF
 * Name: SCDCommunicationCheck
 * Desc: Communication元素检测
 * Ref: GDW1396
 */

#ifndef _SCDCommunicationCheck_H
#define _SCDCommunicationCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDCommunicationCheck : public CSingleInstance<CSCDCommunicationCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDCommunicationCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDCommunicationCheck_H */
