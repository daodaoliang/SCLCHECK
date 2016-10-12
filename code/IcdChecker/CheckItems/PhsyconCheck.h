/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: ED195B
 * Name: PhsyconCheck
 * Desc: Communication下PhysConn配置的物理端口插头类型、端口号、接口类型等是否符合规范中的命名要求
 * Ref: GDW1396
 */

#ifndef _PhsyconCheck_H
#define _PhsyconCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CPhsyconCheck : public CSingleInstance<CPhsyconCheck>, CNoCopy, public CCheckItemBase
{
public:
    CPhsyconCheck();

public:
    virtual bool CheckInternal();

    void CheckPortType_InCacheMgr(std::wstring sType, std::wstring sValue, int offset);
};

#endif /* _PhsyconCheck_H */
