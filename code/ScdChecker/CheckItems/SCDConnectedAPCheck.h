/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 72417E
 * Name: SCDConnectedAPCheck
 * Desc: ConnectedAP元素检测
 * Ref: GDW1396
 */

#ifndef _SCDConnectedAPCheck_H
#define _SCDConnectedAPCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDConnectedAPCheck : public CSingleInstance<CSCDConnectedAPCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDConnectedAPCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDConnectedAPCheck_H */
