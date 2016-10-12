/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 58E4DA
 * Name: FcdaLdValidCheck
 * Desc: 数据集中包含的FCDA不应跨LDevice
 * Ref: Q/GDW 1396-2012 第7.1.3节
 */

#ifndef _FcdaLdValidCheck_H
#define _FcdaLdValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CFcdaLdValidCheck : public CSingleInstance<CFcdaLdValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CFcdaLdValidCheck();

public:
    virtual bool CheckInternal();
    static void CheckCrossLDevice(FcdaCache& fcda, std::wstring sLd);
};

#endif /* _FcdaLdValidCheck_H */
