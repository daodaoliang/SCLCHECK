/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: EA5BBA
 * Name: FcdaValidCheck
 * Desc: 数据集中包含的FCDA的引用有效性检查（包括报告控制块及其他控制块）
 * Ref: 工程实践（SCD检测已有逻辑）
 */

#ifndef _FcdaValidCheck_H
#define _FcdaValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CFcdaValidCheck : public CSingleInstance<CFcdaValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CFcdaValidCheck();

public:
    virtual bool CheckInternal();

    static bool CheckDaType(int offset, std::vector<wstring>& vecDaName, std::wstring sDaType);
    static bool CheckDoType(int offset, std::vector<wstring>& vecDoName, std::wstring sDoType);
    static bool CheckValid_InCacheMgr(FcdaCache& fcda, LDeviceCache& lDevice);
};

#endif /* _FcdaValidCheck_H */
