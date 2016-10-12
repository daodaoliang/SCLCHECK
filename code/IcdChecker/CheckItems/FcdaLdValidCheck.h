/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 58E4DA
 * Name: FcdaLdValidCheck
 * Desc: ���ݼ��а�����FCDA��Ӧ��LDevice
 * Ref: Q/GDW 1396-2012 ��7.1.3��
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
