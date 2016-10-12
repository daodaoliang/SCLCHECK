/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 51A518
 * Name: RedInputsCheck
 * Desc: IED中不应包含Inputs这类代表GOOSE/SV连线的信息。
 * Ref: 工程实践
 */

#ifndef _RedInputsCheck_H
#define _RedInputsCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CRedInputsCheck : public CSingleInstance<CRedInputsCheck>, CNoCopy, public CCheckItemBase
{
public:
    CRedInputsCheck();

public:
    virtual bool CheckInternal();

    static void CheckInputs_InCacheMgr(int offset);
};

#endif /* _RedInputsCheck_H */
